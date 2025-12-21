#ifndef BOND_RISK_SERVICE_HPP
#define BOND_RISK_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "products.hpp"
#include "riskservice.hpp"
#include "soa.hpp"

class BondRiskService final : public RiskService<Bond>,
                              public ServiceListener<Position<Bond>> {
public:
    BondRiskService() = default;

    PV01<Bond>& GetData(std::string key) override { return risks_.at(key); }

    void OnMessage(PV01<Bond>& data) override 
    {
        const std::string pid = data.GetProduct().GetProductId();
        auto key = pid;
        risks_.erase(key);
        risks_.emplace(key, PV01<Bond>(data.GetProduct(), data.GetPV01(), data.GetQuantity()));

        for (auto* l : listeners_) l->ProcessUpdate(risks_.at(pid));
    }

    void AddListener(ServiceListener<PV01<Bond>>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<PV01<Bond>>*>& GetListeners() const override 
    {
        return listeners_;
    }

    // RiskService<Bond>
    void AddPosition(Position<Bond>& position) override 
    {
        const Bond& b = position.GetProduct();
        const std::string pid = b.GetProductId();

        // "Realistic PV01" placeholders (USD PV01 per $1mm notional-ish scale).
        // Adjust as you like.
        const double pv01_per_unit = Pv01ForBond(pid);

        // We need aggregate position. The base Position<T> aggregate is a no-op.
        // So compute it from its internal map (we have access in BondPositionService only).
        // Here we approximate by summing three known books. If you store more books, extend.
        long qty = 0;
        {
            std::string b1 = "TRSY1", b2 = "TRSY2", b3 = "TRSY3";
            qty += position.GetPosition(b1);
            qty += position.GetPosition(b2);
            qty += position.GetPosition(b3);
        }

        PV01<Bond> pv(b, pv01_per_unit, qty);
        risks_.erase(pid);
        risks_.emplace(pid, PV01<Bond>(pv.GetProduct(), pv.GetPV01(), pv.GetQuantity()));

        for (auto* l : listeners_) l->ProcessUpdate(risks_.at(pid));
    }

    const PV01<BucketedSector<Bond>>& GetBucketedRisk(const BucketedSector<Bond>& sector) const override 
    {
        // Compute bucket PV01 = sum(pv01_i * qty_i) with pv01 stored already as "pv01 per unit".
        // Return value is stored in mutable cache_ so we can return a const ref.
        double pv01_sum = 0.0;
        long qty_sum = 0;

        for (const auto& prod : sector.GetProducts()) 
        {
            const std::string pid = prod.GetProductId();
            auto it = risks_.find(pid);
            if (it == risks_.end()) continue;
            pv01_sum += it->second.GetPV01() * static_cast<double>(it->second.GetQuantity());
            qty_sum += it->second.GetQuantity();
        }

        cached_bucket_ = PV01<BucketedSector<Bond>>(sector, pv01_sum, qty_sum);
        return cached_bucket_;
    }

  // ServiceListener<Position<Bond>>
  void ProcessAdd(Position<Bond>& p) override { AddPosition(p); }
  void ProcessRemove(Position<Bond>&) override {}
  void ProcessUpdate(Position<Bond>& p) override { AddPosition(p); }

private:
    static double Pv01ForBond(const std::string& product_id) 
    {
        // Simple mapping by tenor label in the product id (your ids will be CUSIPs; you can map those).
        // Replace with your chosen "realistic PV01 values".
        if (product_id.find("2") != std::string::npos) return 0.020;
        if (product_id.find("3") != std::string::npos) return 0.028;
        if (product_id.find("5") != std::string::npos) return 0.045;
        if (product_id.find("7") != std::string::npos) return 0.060;
        if (product_id.find("10") != std::string::npos) return 0.085;
        if (product_id.find("20") != std::string::npos) return 0.120;
        if (product_id.find("30") != std::string::npos) return 0.150;
        return 0.050;
    }

    std::map<std::string, PV01<Bond>> risks_;
    std::vector<ServiceListener<PV01<Bond>>*> listeners_;
    mutable PV01<BucketedSector<Bond>> cached_bucket_ = PV01<BucketedSector<Bond>>(BucketedSector<Bond>({}, "EMPTY"), 0.0, 0);
};

#endif
