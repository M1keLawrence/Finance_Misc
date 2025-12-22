#ifndef BOND_HISTORICAL_DATA_SERVICE_HPP
#define BOND_HISTORICAL_DATA_SERVICE_HPP

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "executionservice.hpp"
#include "historicaldataservice.hpp"
#include "inquiryservice.hpp"
#include "products.hpp"
#include "riskservice.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"

// File-based historical connectors (PositionFileConnector, RiskFileConnector, etc.)
#include "HistoricalWriters.hpp"

// -----------------------------------------------------------------------------
// Listener that persists every update into a HistoricalDataService.
// For compatibility with prior code that used PersistToHistoricalListener<...>.
// -----------------------------------------------------------------------------
template <typename T>
class PersistToHistoricalListener final : public ServiceListener<T> {
 public:
  explicit PersistToHistoricalListener(HistoricalDataService<T>& svc) : svc_(svc) {}

  void ProcessAdd(T& d) override { Persist(d); }
  void ProcessRemove(T& d) override { Persist(d); }
  void ProcessUpdate(T& d) override { Persist(d); }

 private:
  void Persist(const T& d) { svc_.PersistData(Key(d), d); }

  // Default keys for the supported Bond types
  static std::string Key(const Position<Bond>& p) { return p.GetProduct().GetProductId(); }
  static std::string Key(const Position<BucketedSector<Bond>>& p) { return p.GetProduct().GetName(); }

  static std::string Key(const PV01<Bond>& r) { return r.GetProduct().GetProductId(); }
  static std::string Key(const PV01<BucketedSector<Bond>>& r) { return r.GetProduct().GetName(); }

  static std::string Key(const ExecutionOrder<Bond>& e) { return e.GetProduct().GetProductId(); }
  static std::string Key(const PriceStream<Bond>& s) { return s.GetProduct().GetProductId(); }
  static std::string Key(const Inquiry<Bond>& i) { return i.GetInquiryId(); }

  HistoricalDataService<T>& svc_;
};

// -----------------------------------------------------------------------------
// Historical data service base that persists to a Connector<T>.
//
// This version supports:
//  - construction from an explicit Connector<T>* (you manage its lifetime), OR
//  - construction from a filename, using the file connectors in HistoricalWriters.hpp
// -----------------------------------------------------------------------------
template <typename T>
class BondHistoricalDataServiceBase : public HistoricalDataService<T> {
 public:
  // Construct with an externally owned connector
  explicit BondHistoricalDataServiceBase(Connector<T>* c) : connector_(c) {}

  // Construct with an internally owned file connector
  explicit BondHistoricalDataServiceBase(const std::string& filename)
      : owned_connector_(MakeFileConnector(filename)), connector_(owned_connector_.get()) {}

  // Service interface required by Service<K,V>
  T& GetData(std::string) override {
    if (!last_) throw std::runtime_error("BondHistoricalDataServiceBase: no data yet");
    return *last_;
  }

  void OnMessage(T& data) override { last_ = std::make_unique<T>(data); }

  void AddListener(ServiceListener<T>*) override {}
  const std::vector<ServiceListener<T>*>& GetListeners() const override {
    static std::vector<ServiceListener<T>*> empty;
    return empty;
  }

  // Persist data to a store
  void PersistData(std::string /*persistKey*/, const T& data) override {
    if (!connector_) return;

    // Connector base expects non-const ref, so copy.
    T tmp = data;
    connector_->Publish(tmp);
    last_ = std::make_unique<T>(tmp);
  }

 private:
  // Factory for file connectors based on T. Extend here if you add new persisted types.
  static std::unique_ptr<Connector<T>> MakeFileConnector(const std::string& filename) {
    if constexpr (std::is_same_v<T, Position<Bond>>) {
      return std::make_unique<PositionFileConnector<Bond>>(filename);
    } else if constexpr (std::is_same_v<T, Position<BucketedSector<Bond>>>) {
      return std::make_unique<BucketPositionFileConnector<Bond>>(filename);
    } else if constexpr (std::is_same_v<T, PV01<Bond>>) {
      return std::make_unique<RiskFileConnector<Bond>>(filename);
    } else if constexpr (std::is_same_v<T, PV01<BucketedSector<Bond>>>) {
      return std::make_unique<BucketRiskFileConnector<Bond>>(filename);
    } else if constexpr (std::is_same_v<T, ExecutionOrder<Bond>>) {
      return std::make_unique<ExecutionFileConnector<Bond>>(filename);
    } else if constexpr (std::is_same_v<T, PriceStream<Bond>>) {
      return std::make_unique<StreamingFileConnector<Bond>>(filename);
    } else if constexpr (std::is_same_v<T, Inquiry<Bond>>) {
      return std::make_unique<InquiryFileConnector<Bond>>(filename);
    } else {
      static_assert(!sizeof(T), "No file connector defined for this historical type T.");
    }
  }

 private:
  std::unique_ptr<Connector<T>> owned_connector_;
  Connector<T>* connector_ = nullptr;
  std::unique_ptr<T> last_;
};

// Concrete type aliases you will instantiate:
using BondHistoricalPositionService = BondHistoricalDataServiceBase<Position<Bond>>;
using BondHistoricalBucketedPositionService = BondHistoricalDataServiceBase<Position<BucketedSector<Bond>>>;

using BondHistoricalRiskService = BondHistoricalDataServiceBase<PV01<Bond>>;
using BondHistoricalBucketedRiskService = BondHistoricalDataServiceBase<PV01<BucketedSector<Bond>>>;

using BondHistoricalExecutionService = BondHistoricalDataServiceBase<ExecutionOrder<Bond>>;
using BondHistoricalStreamingService = BondHistoricalDataServiceBase<PriceStream<Bond>>;
using BondHistoricalInquiryService = BondHistoricalDataServiceBase<Inquiry<Bond>>;

#endif
