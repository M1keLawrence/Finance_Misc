#include <iostream>
#include <thread>

#include "BondUniverse.hpp"
// Your bond services
#include "BondTradeBookingService.hpp"
#include "BondPositionService.hpp"
#include "BondRiskService.hpp"
#include "BondPricingService.hpp"
#include "BondMarketDataService.hpp"
#include "BondExecutionService.hpp"
#include "BondStreamingService.hpp"
#include "BondAlgoExecutionService.hpp"
#include "BondAlgoStreamingService.hpp"
#include "GUIService.hpp"
#include "BondInquiryService.hpp"

// Connectors
#include "BondMarketDataShmConnectors.hpp"
#include "BondTypedConnectors.hpp"
#include "HistoricalWriters.hpp"
#include "InquiryQuoteLoopbackConnector.hpp"

// Bridge listeners (adapters)
class TradeToPositionListener final : public ServiceListener<Trade<Bond>> {
 public:
  explicit TradeToPositionListener(BondPositionService& pos) : pos_(pos) {}
  void ProcessAdd(Trade<Bond>& t) override { pos_.AddTrade(t); }
  void ProcessUpdate(Trade<Bond>& t) override { pos_.AddTrade(t); }
  void ProcessRemove(Trade<Bond>&) override {}
 private:
  BondPositionService& pos_;
};

class PositionToRiskListener final : public ServiceListener<Position<Bond>> {
 public:
  explicit PositionToRiskListener(BondRiskService& risk) : risk_(risk) {}
  void ProcessAdd(Position<Bond>& p) override { risk_.AddPosition(p); }
  void ProcessUpdate(Position<Bond>& p) override { risk_.AddPosition(p); }
  void ProcessRemove(Position<Bond>&) override {}
 private:
  BondRiskService& risk_;
};

class AlgoExecToExecutionListener final : public ServiceListener<AlgoExecution> {
 public:
  explicit AlgoExecToExecutionListener(BondExecutionService& exec) : exec_(exec) {}
  void ProcessAdd(AlgoExecution& ae) override {
    ExecutionOrder<Bond> order = ae.GetOrder();
    exec_.ExecuteOrder(order, BROKERTEC);
  }
  void ProcessUpdate(AlgoExecution& ae) override { ProcessAdd(ae); }
  void ProcessRemove(AlgoExecution&) override {}
 private:
  BondExecutionService& exec_;
};

class AlgoStreamToStreamingListener final : public ServiceListener<AlgoStream> {
 public:
  explicit AlgoStreamToStreamingListener(BondStreamingService& stream) : stream_(stream) {}
  void ProcessAdd(AlgoStream& as) override {
    PriceStream<Bond> ps = as.GetPriceStream();
    stream_.PublishPrice(ps);
  }
  void ProcessUpdate(AlgoStream& as) override { ProcessAdd(as); }
  void ProcessRemove(AlgoStream&) override {}
 private:
  BondStreamingService& stream_;
};

class ExecutionToTradeBookingListener final : public ServiceListener<ExecutionOrder<Bond>> {
 public:
  explicit ExecutionToTradeBookingListener(BondTradeBookingService& tb)
      : tb_(tb) {}

  void ProcessAdd(ExecutionOrder<Bond>& eo) override {
    const std::string book = NextBook();
    const std::string trade_id = "EXEC_TRADE_" + std::to_string(seq_++);
    const Side side = (eo.GetOrderType() == MARKET) ? BUY : BUY;  // not used; choose BUY/SELL if desired
    Trade<Bond> trade(eo.GetProduct(), trade_id, eo.GetPrice(), book, eo.GetVisibleQuantity(), side);
    tb_.BookTrade(trade);
  }
  void ProcessUpdate(ExecutionOrder<Bond>& eo) override { ProcessAdd(eo); }
  void ProcessRemove(ExecutionOrder<Bond>&) override {}

 private:
  std::string NextBook() {
    const std::string b = books_[book_idx_];
    book_idx_ = (book_idx_ + 1) % books_.size();
    return b;
  }
  BondTradeBookingService& tb_;
  long seq_ = 1;
  std::size_t book_idx_ = 0;
  const std::vector<std::string> books_ = {"TRSY1", "TRSY2", "TRSY3"};
};

// Historical persistence listeners
template <typename T>
class PersistListener final : public ServiceListener<T> {
 public:
  explicit PersistListener(Connector<T>& conn) : conn_(conn) {}
  void ProcessAdd(T& d) override { conn_.Publish(d); }
  void ProcessUpdate(T& d) override { conn_.Publish(d); }
  void ProcessRemove(T& d) override { conn_.Publish(d); }
 private:
  Connector<T>& conn_;
};

int main() {
  RegisterBondUniverse();

  // ---------- Services ----------
  BondPricingService pricing_svc;
  BondMarketDataService marketdata_svc;
  BondTradeBookingService tradebooking_svc;
  BondPositionService position_svc;
  BondRiskService risk_svc;

  BondAlgoExecutionService algo_exec_svc;
  BondExecutionService execution_svc;

  BondAlgoStreamingService algo_stream_svc;
  BondStreamingService streaming_svc;

  GUIService gui_svc("gui.txt", std::chrono::milliseconds(300));

  BondInquiryService inquiry_svc;

  // ---------- Service wiring ----------
  // TradeBooking -> Position -> Risk
  TradeToPositionListener trade_to_pos(position_svc);
  PositionToRiskListener pos_to_risk(risk_svc);
  tradebooking_svc.AddListener(&trade_to_pos);
  position_svc.AddListener(&pos_to_risk);

  // MarketData -> AlgoExecution -> Execution
  marketdata_svc.AddListener(&algo_exec_svc);
  AlgoExecToExecutionListener algoexec_to_exec(execution_svc);
  algo_exec_svc.AddListener(&algoexec_to_exec);

  // Execution -> TradeBooking (cycle books)
  ExecutionToTradeBookingListener exec_to_tb(tradebooking_svc);
  execution_svc.AddListener(&exec_to_tb);

  // Pricing -> AlgoStreaming -> Streaming
  pricing_svc.AddListener(&algo_stream_svc);
  AlgoStreamToStreamingListener algostream_to_stream(streaming_svc);
  algo_stream_svc.AddListener(&algostream_to_stream);

  // Pricing -> GUI (throttled)
  pricing_svc.AddListener(&gui_svc);

  // Inquiry: loopback connector handles QUOTED/DONE via OnMessage
  InquiryQuoteLoopbackConnector<BondInquiryService, Bond> inquiry_loop(inquiry_svc);
  inquiry_svc.SetConnector(&inquiry_loop);

  // ---------- Outbound socket connectors ----------
  auto exec_out = MakeExecutionOutbound("127.0.0.1", 9101);
  execution_svc.SetPublishConnector(&exec_out);

  auto stream_out = MakeStreamingOutbound("127.0.0.1", 9102);
  streaming_svc.SetPublishConnector(&stream_out);

  // ---------- Historical file connectors ----------
  PositionFileConnector<Bond> pos_file("positions.txt");
  RiskFileConnector<Bond> risk_file("risk.txt");
  ExecutionFileConnector<Bond> exec_file("executions.txt");
  StreamingFileConnector<Bond> stream_file("streaming.txt");
  InquiryFileConnector<Bond> inq_file("allinquiries.txt");

  PersistListener<Position<Bond>> persist_pos(pos_file);
  PersistListener<PV01<Bond>> persist_risk(risk_file);
  PersistListener<ExecutionOrder<Bond>> persist_exec(exec_file);
  PersistListener<PriceStream<Bond>> persist_stream(stream_file);
  PersistListener<Inquiry<Bond>> persist_inq(inq_file);

  position_svc.AddListener(&persist_pos);
  risk_svc.AddListener(&persist_risk);
  execution_svc.AddListener(&persist_exec);
  streaming_svc.AddListener(&persist_stream);
  inquiry_svc.AddListener(&persist_inq);

  // ---------- Inbound connectors ----------
  // SHM market data subscriber (trading system side)
  BondMarketDataShmSubscriber<BondMarketDataService> md_in(marketdata_svc, "BOND_MD_SHM");

  // Socket inbound pricing, trades, inquiries
  auto px_in = MakePricingInbound(pricing_svc, 9001);
  auto tr_in = MakeTradesInbound(tradebooking_svc, 9002);
  auto iq_in = MakeInquiriesInbound(inquiry_svc, 9003);

  // ---------- Run inbound feeds (threads) ----------
  std::thread t_md([&] { md_in.Subscribe(); });
  std::thread t_px([&] { px_in.Subscribe(); });
  std::thread t_tr([&] { tr_in.Subscribe(); });
  std::thread t_iq([&] { iq_in.Subscribe(); });
  std::cout << "Trading system running.\n"
            << "Ports: prices=9001 trades=9002 inquiries=9003\n"
            << "Outbound: executions=9101 streaming=9102\n"
            << "Market data SHM name: BOND_MD_SHM\n";

  t_md.join();
  t_px.join();
  t_tr.join();
  t_iq.join();
  return 0;
}
