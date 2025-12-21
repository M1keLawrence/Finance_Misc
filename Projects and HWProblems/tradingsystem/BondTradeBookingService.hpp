#ifndef BOND_TRADE_BOOKING_SERVICE_HPP
#define BOND_TRADE_BOOKING_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "products.hpp"
#include "soa.hpp"
#include "tradebookingservice.hpp"

class BondTradeBookingService final : public TradeBookingService<Bond> {
public:
    BondTradeBookingService() = default;

    Trade<Bond>& GetData(std::string key) override { return trades_.at(key); }

    void OnMessage(Trade<Bond>& data) override 
    {
        auto key = data.GetTradeId();
        trades_.erase(key);
        trades_.emplace(key, Trade<Bond>(data.GetProduct(), data.GetTradeId(),
                                 data.GetPrice(), data.GetBook(),
                                 data.GetQuantity(), data.GetSide()));

        for (auto* l : listeners_) l->ProcessAdd(trades_.at(data.GetTradeId()));
    }

    void AddListener(ServiceListener<Trade<Bond>>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<Trade<Bond>>*>& GetListeners() const override 
    {
        return listeners_;
    }

    void BookTrade(const Trade<Bond>& trade) override 
    {
        const std::string key = trade.GetTradeId();
        trades_.erase(key);
        trades_.emplace(key, Trade<Bond>(trade.GetProduct(),
                                        trade.GetTradeId(),
                                        trade.GetPrice(),
                                        trade.GetBook(),
                                        trade.GetQuantity(),
                                        trade.GetSide()));

        Trade<Bond>& stored = trades_.at(trade.GetTradeId());
        for (auto* l : listeners_) l->ProcessAdd(stored);
    }

private:
    std::map<std::string, Trade<Bond>> trades_;
    std::vector<ServiceListener<Trade<Bond>>*> listeners_;
};

#endif
