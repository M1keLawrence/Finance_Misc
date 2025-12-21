#ifndef HISTORICAL_WRITERS_HPP
#define HISTORICAL_WRITERS_HPP

#include <chrono>
#include <fstream>
#include <string>
#include <vector>

#include "BondPriceUtils.hpp"
#include "products.hpp"
#include "riskservice.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"
#include "executionservice.hpp"
#include "inquiryservice.hpp"

// Timestamp helper
inline long long NowMs() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

// ---------- Positions writer ----------
template <typename T>
class PositionFileConnector final : public Connector<Position<T>> {
 public:
  explicit PositionFileConnector(const std::string& filename)
      : out_(filename, std::ios::out) {}

  void Publish(Position<T>& p) override {
    const std::string pid = p.GetProduct().GetProductId();

    // Persist each book + aggregate
    std::vector<std::string> books = {"TRSY1", "TRSY2", "TRSY3"};
    long agg = 0;
    for (auto& b : books) {
      std::string bb = b;
      long q = p.GetPosition(bb);
      agg += q;
      out_ << NowMs() << "," << pid << "," << b << "," << q << "\n";
    }
    out_ << NowMs() << "," << pid << "," << "AGG" << "," << agg << "\n";
    out_.flush();
  }

 private:
  std::ofstream out_;
};

// ---------- Risk writer ----------
template <typename T>
class RiskFileConnector final : public Connector<PV01<T>> {
 public:
  explicit RiskFileConnector(const std::string& filename)
      : out_(filename, std::ios::out) {}

  void Publish(PV01<T>& r) override {
    out_ << NowMs() << "," << r.GetProduct().GetProductId()
         << "," << r.GetPV01() << "," << r.GetQuantity() << "\n";
    out_.flush();
  }

 private:
  std::ofstream out_;
};

// Bucket sector risk writer (FrontEnd/Belly/LongEnd)
template <typename T>
class BucketRiskFileConnector final : public Connector<PV01<BucketedSector<T>>> {
 public:
  explicit BucketRiskFileConnector(const std::string& filename)
      : out_(filename, std::ios::out) {}

  void Publish(PV01<BucketedSector<T>>& r) override {
    out_ << NowMs() << "," << r.GetProduct().GetName()
         << "," << r.GetPV01() << "," << r.GetQuantity() << "\n";
    out_.flush();
  }

 private:
  std::ofstream out_;
};

// ---------- Executions writer ----------
template <typename T>
class ExecutionFileConnector final : public Connector<ExecutionOrder<T>> {
 public:
  explicit ExecutionFileConnector(const std::string& filename)
      : out_(filename, std::ios::out) {}

  void Publish(ExecutionOrder<T>& e) override {
    out_ << NowMs() << "," << e.GetProduct().GetProductId()
         << "," << e.GetOrderId()
         << "," << static_cast<int>(e.GetOrderType())
         << "," << FormatPriceFractional(e.GetPrice())
         << "," << e.GetVisibleQuantity()
         << "," << e.GetHiddenQuantity()
         << "," << e.GetParentOrderId()
         << "," << (e.IsChildOrder() ? 1 : 0)
         << "\n";
    out_.flush();
  }

 private:
  std::ofstream out_;
};

// ---------- Streaming writer ----------
template <typename T>
class StreamingFileConnector final : public Connector<PriceStream<T>> {
 public:
  explicit StreamingFileConnector(const std::string& filename)
      : out_(filename, std::ios::out) {}

  void Publish(PriceStream<T>& ps) override {
    const auto& bid = ps.GetBidOrder();
    const auto& off = ps.GetOfferOrder();

    out_ << NowMs() << "," << ps.GetProduct().GetProductId()
         << "," << FormatPriceFractional(bid.GetPrice())
         << "," << bid.GetVisibleQuantity()
         << "," << bid.GetHiddenQuantity()
         << "," << FormatPriceFractional(off.GetPrice())
         << "," << off.GetVisibleQuantity()
         << "," << off.GetHiddenQuantity()
         << "\n";
    out_.flush();
  }

 private:
  std::ofstream out_;
};

// ---------- Inquiry writer ----------
template <typename T>
class InquiryFileConnector final : public Connector<Inquiry<T>> {
 public:
  explicit InquiryFileConnector(const std::string& filename)
      : out_(filename, std::ios::out) {}

  void Publish(Inquiry<T>& i) override {
    out_ << NowMs() << "," << i.GetInquiryId()
         << "," << i.GetProduct().GetProductId()
         << "," << (i.GetSide() == BUY ? "BUY" : "SELL")
         << "," << i.GetQuantity()
         << "," << FormatPriceFractional(i.GetPrice())
         << "," << static_cast<int>(i.GetState())
         << "\n";
    out_.flush();
  }

 private:
  std::ofstream out_;
};

#endif