#include <iostream>
#include <thread>
#include <unordered_map>

#include "BondUniverse.hpp"

// Services
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

// Historical persistence (UPDATED)
#include "BondHistoricalDataService.hpp"

// Connectors
#include "BondMarketDataShmConnectors.hpp"
#include "BondTypedConnectors.hpp"
#include "InquiryQuoteLoopbackConnector.hpp"
#include "BondProductRepository.hpp"

// --------- Bridge listeners (adapters) ----------
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
  explicit ExecutionToTradeBookingListener(BondTradeBookingService& tb) : tb_(tb) {}

  void ProcessAdd(ExecutionOrder<Bond>& eo) override {
    // Convert executions to trades so PositionService gets updated.
    static long seq = 1;
    const std::string trade_id = "T" + std::to_string(seq++);
    const std::string book = "TRSY1";
    Side side = (eo.GetSide() == BID ? BUY : SELL);
    Trade<Bond> trade(eo.GetProduct(), trade_id, eo.GetPrice(), book, eo.GetVisibleQuantity(), side);
    tb_.BookTrade(trade);
  }
  void ProcessUpdate(ExecutionOrder<Bond>& eo) override { ProcessAdd(eo); }
  void ProcessRemove(ExecutionOrder<Bond>&) override {}

 private:
  BondTradeBookingService& tb_;
};

// --------- Bucket helpers ----------
static std::string BucketNameForProductId(const std::string& pid) {
  if (pid == "2Y" || pid == "3Y") return "FrontEnd";
  if (pid == "5Y" || pid == "7Y" || pid == "10Y") return "Belly";
  if (pid == "20Y" || pid == "30Y") return "LongEnd";
  return "Unknown";
}

static std::vector<Bond> BucketProducts(const std::string& bucket) {
  auto& repo = BondProductRepository::Instance();
  if (bucket == "FrontEnd") return {repo.Get("2Y"), repo.Get("3Y")};
  if (bucket == "Belly") return {repo.Get("5Y"), repo.Get("7Y"), repo.Get("10Y")};
  if (bucket == "LongEnd") return {repo.Get("20Y"), repo.Get("30Y")};
  return {};
}

// --------- Bucketed persistence listeners ----------
// Maintains running aggregate by bucket and persists Position<BucketedSector<Bond>>.
class BucketedPositionPersistListener final : public ServiceListener<Position<Bond>> {
 public:
  explicit BucketedPositionPersistListener(BondHistoricalBucketedPositionService& hist)
      : hist_(hist) {}

  void ProcessAdd(Position<Bond>& p) override { OnPos(p); }
  void ProcessUpdate(Position<Bond>& p) override { OnPos(p); }
  void ProcessRemove(Position<Bond>& p) override { OnPos(p); }

 private:
  void OnPos(const Position<Bond>& p) {
    const std::string pid = p.GetProduct().GetProductId();
    const std::string bucket = BucketNameForProductId(pid);
    if (bucket == "Unknown") return;

    const long new_qty = p.GetAggregatePosition();
    const long old_qty = last_qty_by_pid_[pid];
    const long delta = new_qty - old_qty;
    last_qty_by_pid_[pid] = new_qty;

    bucket_qty_[bucket] += delta;

    BucketedSector<Bond> sector(BucketProducts(bucket), bucket);
    Position<BucketedSector<Bond>> bucket_pos(sector);
    bucket_pos.SetPosition("AGG", bucket_qty_[bucket]);

    hist_.PersistData(bucket, bucket_pos);
  }

  BondHistoricalBucketedPositionService& hist_;
  std::unordered_map<std::string, long> last_qty_by_pid_;
  std::unordered_map<std::string, long> bucket_qty_;
};

// Maintains running aggregate by bucket and persists PV01<BucketedSector<Bond>>.
class BucketedRiskPersistListener final : public ServiceListener<PV01<Bond>> {
 public:
  explicit BucketedRiskPersistListener(BondHistoricalBucketedRiskService& hist)
      : hist_(hist) {}

  void ProcessAdd(PV01<Bond>& r) override { OnRisk(r); }
  void ProcessUpdate(PV01<Bond>& r) override { OnRisk(r); }
  void ProcessRemove(PV01<Bond>& r) override { OnRisk(r); }

 private:
  void OnRisk(const PV01<Bond>& r) {
    const std::string pid = r.GetProduct().GetProductId();
    const std::string bucket = BucketNameForProductId(pid);
    if (bucket == "Unknown") return;

    // contribution = pv01_per_unit * qty
    const double new_contrib = r.GetPV01() * static_cast<double>(r.GetQuantity());
    const double old_contrib = last_contrib_by_pid_[pid];
    const double delta = new_contrib - old_contrib;
    last_contrib_by_pid_[pid] = new_contrib;

    // quantities
    const long new_qty = r.GetQuantity();
    const long old_qty = last_qty_by_pid_[pid];
    last_qty_by_pid_[pid] = new_qty;

    bucket_pv01_[bucket] += delta;
    bucket_qty_[bucket] += (new_qty - old_qty);

    BucketedSector<Bond> sector(BucketProducts(bucket), bucket);
    PV01<BucketedSector<Bond>> bucket_risk(sector, bucket_pv01_[bucket], bucket_qty_[bucket]);

    hist_.PersistData(bucket, bucket_risk);
  }

  BondHistoricalBucketedRiskService& hist_;
  std::unordered_map<std::string, double> last_contrib_by_pid_;
  std::unordered_map<std::string, long> last_qty_by_pid_;
  std::unordered_map<std::string, double> bucket_pv01_;
  std::unordered_map<std::string, long> bucket_qty_;
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

  // MarketData -> AlgoExecution -> Execution -> TradeBooking
  marketdata_svc.AddListener(&algo_exec_svc);
  AlgoExecToExecutionListener algoexec_to_exec(execution_svc);
  algo_exec_svc.AddListener(&algoexec_to_exec);

  ExecutionToTradeBookingListener exec_to_tb(tradebooking_svc);
  execution_svc.AddListener(&exec_to_tb);

  // Pricing -> AlgoStreaming -> Streaming
  pricing_svc.AddListener(&algo_stream_svc);
  AlgoStreamToStreamingListener algostream_to_stream(streaming_svc);
  algo_stream_svc.AddListener(&algostream_to_stream);

  // Pricing -> GUI
  pricing_svc.AddListener(&gui_svc);

  // Inquiry -> loopback (quote -> response)
  InquiryQuoteLoopbackConnector<BondInquiryService, Bond> inq_loopback(inquiry_svc);
  inquiry_svc.SetConnector(&inq_loopback);

  // ---------- Historical persistence ----------
  BondHistoricalPositionService hist_pos("positions.txt");
  BondHistoricalBucketedPositionService hist_bpos("positions_bucketed.txt");
  BondHistoricalRiskService hist_risk("risk.txt");
  BondHistoricalBucketedRiskService hist_brisk("risk_bucketed.txt");
  BondHistoricalExecutionService hist_exec("executions.txt");
  BondHistoricalStreamingService hist_stream("streaming.txt");
  BondHistoricalInquiryService hist_inq("allinquiries.txt");

  PersistToHistoricalListener<Position<Bond>> persist_pos(hist_pos);
  PersistToHistoricalListener<PV01<Bond>> persist_risk(hist_risk);
  PersistToHistoricalListener<ExecutionOrder<Bond>> persist_exec(hist_exec);
  PersistToHistoricalListener<PriceStream<Bond>> persist_stream(hist_stream);
  PersistToHistoricalListener<Inquiry<Bond>> persist_inq(hist_inq);

  BucketedPositionPersistListener persist_bpos(hist_bpos);
  BucketedRiskPersistListener persist_brisk(hist_brisk);

  position_svc.AddListener(&persist_pos);
  position_svc.AddListener(&persist_bpos);
  risk_svc.AddListener(&persist_risk);
  risk_svc.AddListener(&persist_brisk);
  execution_svc.AddListener(&persist_exec);
  streaming_svc.AddListener(&persist_stream);
  inquiry_svc.AddListener(&persist_inq);

  // ---------- Inbound connectors ----------
  BondMarketDataShmSubscriber<BondMarketDataService> md_in(marketdata_svc, "BOND_MD_SHM");
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
