#ifndef BOND_POSITION_SERVICE_HPP
#define BOND_POSITION_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "products.hpp"
#include "soa.hpp"
#include "tradebookingservice.hpp"

// Hack to allow updating Position<T>::positions (private in instructor file)
#define private public
#include "positionservice.hpp"
#undef private

class BondPositionService final : public PositionService<Bond>,
                                 public ServiceListener<Trade<Bond>> {
public:
    BondPositionService() = default;

    Position<Bond>& GetData(std::string key) override { return positions_.at(key); }

    void OnMessage(Position<Bond>& data) override 
    {
        const std::string pid = data.GetProduct().GetProductId();
        auto it = positions_.find(pid);
        if (it == positions_.end()) {
        it = positions_.emplace(pid, Position<Bond>(data.GetProduct())).first;
        }
        Position<Bond>& pos = it->second;
        for (auto* l : listeners_) l->ProcessUpdate(positions_.at(pid));
    }

    void AddListener(ServiceListener<Position<Bond>>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<Position<Bond>>*>& GetListeners() const override 
    {
        return listeners_;
    }

    void AddTrade(const Trade<Bond>& trade) override 
    {
        const std::string pid = trade.GetProduct().GetProductId();
        if (positions_.find(pid) == positions_.end()) positions_.emplace(pid, Position<Bond>(trade.GetProduct()));

        Position<Bond>& pos = positions_.at(pid);
        long signed_qty = (trade.GetSide() == BUY) ? trade.GetQuantity() : -trade.GetQuantity();
        pos.positions[trade.GetBook()] += signed_qty;  // NOLINT (private access intentionally enabled)

        for (auto* l : listeners_) l->ProcessUpdate(pos);
    }

    // ServiceListener<Trade<Bond>>
    void ProcessAdd(Trade<Bond>& trade) override { AddTrade(trade); }
    void ProcessRemove(Trade<Bond>&) override {}
    void ProcessUpdate(Trade<Bond>& trade) override { AddTrade(trade); }

private:
    std::map<std::string, Position<Bond>> positions_;
    std::vector<ServiceListener<Position<Bond>>*> listeners_;
};

#endif
