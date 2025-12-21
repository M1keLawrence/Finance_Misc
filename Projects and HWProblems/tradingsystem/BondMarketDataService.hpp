#ifndef BOND_MARKET_DATA_SERVICE_HPP
#define BOND_MARKET_DATA_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "marketdataservice.hpp"
#include "products.hpp"
#include "soa.hpp"

class BondMarketDataService final : public MarketDataService<Bond> {
public:
    BondMarketDataService() = default;

    OrderBook<Bond>& GetData(std::string key) override { return books_.at(key); }

    void OnMessage(OrderBook<Bond>& data) override 
    {
        const std::string pid = data.GetProduct().GetProductId();
        // books_
        books_.erase(pid);
        books_.emplace(pid, OrderBook<Bond>(data.GetProduct(), data.GetBidStack(), data.GetOfferStack()));

        RebuildBest(pid);
        for (auto* l : listeners_) l->ProcessUpdate(books_.at(pid));
    }

    void AddListener(ServiceListener<OrderBook<Bond>>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<OrderBook<Bond>>*>& GetListeners() const override 
    {
        return listeners_;
    }

    const BidOffer& GetBestBidOffer(const std::string& productId) override 
    {
        return best_.at(productId);
    }

    const OrderBook<Bond>& AggregateDepth(const std::string& productId) override 
    {
        return books_.at(productId);
    }

private:
    void RebuildBest(const std::string& pid) 
    {
        const auto& ob = books_.at(pid);
        const auto& bids = ob.GetBidStack();
        const auto& offers = ob.GetOfferStack();
        if (!bids.empty() && !offers.empty()) {
            best_.erase(pid);
            best_.emplace(pid, BidOffer(bids.front(), offers.front()));
        }
    }

    std::map<std::string, OrderBook<Bond>> books_;
    std::map<std::string, BidOffer> best_;
    std::vector<ServiceListener<OrderBook<Bond>>*> listeners_;
};

#endif
