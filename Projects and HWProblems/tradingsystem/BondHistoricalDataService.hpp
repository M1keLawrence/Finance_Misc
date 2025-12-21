#ifndef BOND_HISTORICAL_DATA_SERVICE_HPP
#define BOND_HISTORICAL_DATA_SERVICE_HPP

#include <string>

#include "executionservice.hpp"
#include "historicaldataservice.hpp"
#include "inquiryservice.hpp"
#include "products.hpp"
#include "riskservice.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"

template <typename T>
class BondPersistListener final : public ServiceListener<T> {
public:
    explicit BondPersistListener(HistoricalDataService<T>& svc) : svc_(svc) {}

    void ProcessAdd(T& d) override { Persist(d); }
    void ProcessRemove(T& d) override { Persist(d); }
    void ProcessUpdate(T& d) override { Persist(d); }

private:
    void Persist(const T& d) 
    {
        // persistKey is chosen by derived specializations (see below)
        svc_.PersistData(Key(d), d);
    }

    static std::string Key(const Position<Bond>& p) { return p.GetProduct().GetProductId(); }
    static std::string Key(const PV01<Bond>& r) { return r.GetProduct().GetProductId(); }
    static std::string Key(const ExecutionOrder<Bond>& e) { return e.GetProduct().GetProductId(); }
    static std::string Key(const PriceStream<Bond>& s) { return s.GetProduct().GetProductId(); }
    static std::string Key(const Inquiry<Bond>& i) { return i.GetInquiryId(); }

    HistoricalDataService<T>& svc_;
};

template <typename T>
class BondHistoricalDataServiceBase : public HistoricalDataService<T> {
public:
    explicit BondHistoricalDataServiceBase(Connector<T>* c) : connector_(c) {}

    // Service interface required by Service<K,V> base
    T& GetData(std::string) override { return last_; }

    void OnMessage(T& data) override { last_ = data; }

    void AddListener(ServiceListener<T>*) override {}

    const std::vector<ServiceListener<T>*>& GetListeners() const override 
    {
        static std::vector<ServiceListener<T>*> empty;
        return empty;
    }

    void PersistData(std::string, const T& data) override 
    {
        if (!connector_) return;
        // Connector base expects non-const ref, so copy.
        T tmp = data;
        connector_->Publish(tmp);
        last_ = tmp;
    }

private:
    Connector<T>* connector_ = nullptr;
    T last_ = T();  // requires default-constructible; most are, except template instantiations may need adaptation
    };

// Concrete type aliases you will instantiate with output connectors:
using BondHistoricalPositionService = BondHistoricalDataServiceBase<Position<Bond>>;
using BondHistoricalRiskService = BondHistoricalDataServiceBase<PV01<Bond>>;
using BondHistoricalExecutionService = BondHistoricalDataServiceBase<ExecutionOrder<Bond>>;
using BondHistoricalStreamingService = BondHistoricalDataServiceBase<PriceStream<Bond>>;
using BondHistoricalInquiryService = BondHistoricalDataServiceBase<Inquiry<Bond>>;

#endif
