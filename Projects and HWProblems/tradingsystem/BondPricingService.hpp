#ifndef BOND_PRICING_SERVICE_HPP
#define BOND_PRICING_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "pricingservice.hpp"
#include "products.hpp"
#include "soa.hpp"

class BondPricingService final : public PricingService<Bond> {
public:
    BondPricingService() = default;

    Price<Bond>& GetData(std::string key) override { return prices_.at(key); }

    void OnMessage(Price<Bond>& data) override 
    {
        const std::string pid = data.GetProduct().GetProductId();
        prices_.erase(pid);
        prices_.emplace(pid, Price<Bond>(data.GetProduct(), data.GetMid(), data.GetBidOfferSpread()));
        for (auto* l : listeners_) l->ProcessUpdate(prices_.at(pid));
    }

    void AddListener(ServiceListener<Price<Bond>>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<Price<Bond>>*>& GetListeners() const override 
    {
        return listeners_;
    }

private:
    std::map<std::string, Price<Bond>> prices_;
    std::vector<ServiceListener<Price<Bond>>*> listeners_;
};

#endif
