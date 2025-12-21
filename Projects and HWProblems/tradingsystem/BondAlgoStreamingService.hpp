#ifndef BOND_ALGO_STREAMING_SERVICE_HPP
#define BOND_ALGO_STREAMING_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "pricingservice.hpp"
#include "products.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"

class AlgoStream 
{
    public:
        explicit AlgoStream(const PriceStream<Bond>& ps) : stream_(ps) {}
        const PriceStream<Bond>& GetPriceStream() const { return stream_; }

    private:
        PriceStream<Bond> stream_;
};

class BondAlgoStreamingService final : public Service<std::string, AlgoStream>,
                                      public ServiceListener<Price<Bond>> {
public:
    BondAlgoStreamingService() = default;

    AlgoStream& GetData(std::string key) override { return streams_.at(key); }

    void OnMessage(AlgoStream& data) override 
    {
        const std::string pid = data.GetPriceStream().GetProduct().GetProductId();
        streams_.erase(pid);
        streams_.emplace(pid, AlgoStream(data.GetPriceStream()));

        for (auto* l : listeners_) l->ProcessUpdate(streams_.at(pid));
    }

    void AddListener(ServiceListener<AlgoStream>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<AlgoStream>*>& GetListeners() const override 
    {
        return listeners_;
    }

    // Listen to BondPricingService
    void ProcessAdd(Price<Bond>& p) override { ProcessUpdate(p); }
    void ProcessRemove(Price<Bond>&) override {}

    void ProcessUpdate(Price<Bond>& p) override 
    {
        const std::string pid = p.GetProduct().GetProductId();

        const double mid = p.GetMid();
        const double spr = p.GetBidOfferSpread();
        const double bid = mid - spr / 2.0;
        const double offer = mid + spr / 2.0;

        const long visible = (toggle_ ? 1'000'000L : 2'000'000L);
        toggle_ = !toggle_;
        const long hidden = 2 * visible;

        PriceStreamOrder bid_order(bid, visible, hidden, BID);
        PriceStreamOrder offer_order(offer, visible, hidden, OFFER);
        PriceStream<Bond> ps(p.GetProduct(), bid_order, offer_order);

        AlgoStream as(ps);
        streams_.erase(pid);
        streams_.emplace(pid, AlgoStream(as.GetPriceStream()));


        AlgoStream& stored = streams_.at(pid);
        for (auto* l : listeners_) l->ProcessAdd(stored);
    }

private:
    std::map<std::string, AlgoStream> streams_;
    std::vector<ServiceListener<AlgoStream>*> listeners_;
    bool toggle_ = true;
};

#endif
