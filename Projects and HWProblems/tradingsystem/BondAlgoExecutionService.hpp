#ifndef BOND_ALGO_EXECUTION_SERVICE_HPP
#define BOND_ALGO_EXECUTION_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "executionservice.hpp"
#include "marketdataservice.hpp"
#include "products.hpp"
#include "soa.hpp"

class AlgoExecution {
public:
    explicit AlgoExecution(const ExecutionOrder<Bond>& order) : order_(order) {}
    const ExecutionOrder<Bond>& GetOrder() const { return order_; }

private:
    ExecutionOrder<Bond> order_;
};

class BondAlgoExecutionService final : public Service<std::string, AlgoExecution>,
                                      public ServiceListener<OrderBook<Bond>> 
{
public:
    BondAlgoExecutionService() = default;

    AlgoExecution& GetData(std::string key) override { return algo_execs_.at(key); }

    void OnMessage(AlgoExecution& data) override 
    {
        const std::string pid = data.GetOrder().GetProduct().GetProductId();
        algo_execs_.erase(pid);
        algo_execs_.emplace(pid, AlgoExecution(data.GetOrder()));

        for (auto* l : listeners_) l->ProcessUpdate(algo_execs_.at(pid));
    }

    void AddListener(ServiceListener<AlgoExecution>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<AlgoExecution>*>& GetListeners() const override 
    {
        return listeners_;
    }

    // Listen to BondMarketDataService
    void ProcessAdd(OrderBook<Bond>& book) override { ProcessUpdate(book); }
    void ProcessRemove(OrderBook<Bond>&) override {}

    void ProcessUpdate(OrderBook<Bond>& book) override 
    {
        // Only aggress when spread is tightest: 1/128 = 0.0078125 in decimal.
        const auto& bids = book.GetBidStack();
        const auto& offers = book.GetOfferStack();
        if (bids.empty() || offers.empty()) return;

        const double bid_px = bids.front().GetPrice();
        const double offer_px = offers.front().GetPrice();
        const double spread = offer_px - bid_px;

        constexpr double kTightSpread = 1.0 / 128.0;
        if (std::abs(spread - kTightSpread) > 1e-12) return;

        const std::string pid = book.GetProduct().GetProductId();

        // Alternate between taking offer (buy) and taking bid (sell)
        const bool buy = next_buy_;
        next_buy_ = !next_buy_;

        const PricingSide side = buy ? OFFER : BID;  // if buy, we aggress OFFER; if sell, aggress BID
        const double px = buy ? offer_px : bid_px;
        const long qty = buy ? offers.front().GetQuantity() : bids.front().GetQuantity();

        // Visible = full size, hidden = 0 for execution in this project spec.
        ExecutionOrder<Bond> order(book.GetProduct(),
                                side,
                                "EXE_" + pid + "_" + std::to_string(seq_++),
                                MARKET,
                                px,
                                qty,
                                0,
                                "",
                                false);

        AlgoExecution algo(order);
        algo_execs_.erase(pid);
        algo_execs_.emplace(pid, AlgoExecution(algo.GetOrder()));


        AlgoExecution& stored = algo_execs_.at(pid);
        for (auto* l : listeners_) l->ProcessAdd(stored);
    }

private:
    std::map<std::string, AlgoExecution> algo_execs_;
    std::vector<ServiceListener<AlgoExecution>*> listeners_;
    bool next_buy_ = true;
    long seq_ = 1;
};

#endif
