#ifndef BOND_EXECUTION_SERVICE_HPP
#define BOND_EXECUTION_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "executionservice.hpp"
#include "products.hpp"
#include "soa.hpp"
#include "tradebookingservice.hpp"

class BondExecutionService final : public ExecutionService<Bond> {
public:
    BondExecutionService() = default;

    void SetPublishConnector(Connector<ExecutionOrder<Bond>>* connector) 
    {
        pub_connector_ = connector;
    }

    ExecutionOrder<Bond>& GetData(std::string key) override { return execs_.at(key); }

    void OnMessage(ExecutionOrder<Bond>&) override 
    {
        // No inbound connector per spec.
    }

    void AddListener(ServiceListener<ExecutionOrder<Bond>>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<ExecutionOrder<Bond>>*>& GetListeners() const override 
    {
        return listeners_;
    }

    void ExecuteOrder(const ExecutionOrder<Bond>& order, Market market) override 
    {
        (void)market;  // Your implementation may route by market.
        const std::string pid = order.GetProduct().GetProductId();
        execs_.erase(pid);
        execs_.emplace(pid, ExecutionOrder<Bond>(order.GetProduct(),
                                                order.GetSide(),
                                                order.GetOrderId(),
                                                order.GetOrderType(),
                                                order.GetPrice(),
                                                order.GetVisibleQuantity(),
                                                order.GetHiddenQuantity(),
                                                order.GetParentOrderId(),
                                                order.IsChildOrder()));


        ExecutionOrder<Bond>& stored = execs_.at(pid);
        for (auto* l : listeners_) l->ProcessAdd(stored);

        if (pub_connector_) pub_connector_->Publish(stored);
    }

private:
    std::map<std::string, ExecutionOrder<Bond>> execs_;
    std::vector<ServiceListener<ExecutionOrder<Bond>>*> listeners_;
    Connector<ExecutionOrder<Bond>>* pub_connector_ = nullptr;
};

#endif