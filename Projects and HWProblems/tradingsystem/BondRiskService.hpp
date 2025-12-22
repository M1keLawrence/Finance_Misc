#ifndef BOND_RISK_SERVICE_HPP
#define BOND_RISK_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "products.hpp"    // Bond + BucketNameForProduct
#include "riskservice.hpp"
#include "soa.hpp"

/**
 * BondRiskService
 *
 * Listens to Position<Bond> updates and computes PV01<Bond> per security.
 * Also supports bucketed PV01 for (FrontEnd, Belly, LongEnd).
 */
class BondRiskService final : public RiskService<Bond>,
                              public ServiceListener<Position<Bond>> {
public:
  BondRiskService() = default;

  // Service<string, PV01<Bond>>
  PV01<Bond>& GetData(std::string key) override { return risks_.at(key); }

  void OnMessage(PV01<Bond>& data) override {
    const std::string pid = data.GetProduct().GetProductId();
    risks_.insert_or_assign(pid, data);
    for (auto* l : listeners_) {
      if (l) l->ProcessUpdate(risks_.at(pid));
    }
  }

  void AddListener(ServiceListener<PV01<Bond>>* listener) override { listeners_.push_back(listener); }

  const std::vector<ServiceListener<PV01<Bond>>*>& GetListeners() const override {
    return listeners_;
  }

  // ServiceListener<Position<Bond>>
  void ProcessAdd(Position<Bond>& position) override { AddPosition(position); }
  void ProcessRemove(Position<Bond>& position) override { AddPosition(position); }
  void ProcessUpdate(Position<Bond>& position) override { AddPosition(position); }

  // RiskService<Bond>
  void AddPosition(Position<Bond>& position) override {
    const Bond& bond = position.GetProduct();
    const std::string pid = bond.GetProductId();

    const long qty = position.GetAggregatePosition();
    const double pv01_per_unit = PV01PerUnit(pid);

    PV01<Bond> pv01(bond, pv01_per_unit, qty);
    OnMessage(pv01);
  }

  const PV01<BucketedSector<Bond>>& GetBucketedRisk(const BucketedSector<Bond>& sector) const override {
    const std::string& bucket_name = sector.GetName();

    double pv01_sum = 0.0;
    long qty_sum = 0;

    for (const auto& kv : risks_) {
      const std::string& pid = kv.first;
      if (BucketNameForProduct(pid) != bucket_name) continue;
      pv01_sum += kv.second.GetPV01() * static_cast<double>(kv.second.GetQuantity());
      qty_sum += kv.second.GetQuantity();
    }

    cached_bucket_ = PV01<BucketedSector<Bond>>(sector, pv01_sum, qty_sum);
    return cached_bucket_;
  }

private:
  static double PV01PerUnit(const std::string& product_id) {
    if (product_id.find("2Y") != std::string::npos) return 0.020;
    if (product_id.find("3Y") != std::string::npos) return 0.030;
    if (product_id.find("5Y") != std::string::npos) return 0.045;
    if (product_id.find("7Y") != std::string::npos) return 0.060;
    if (product_id.find("10Y") != std::string::npos) return 0.085;
    if (product_id.find("20Y") != std::string::npos) return 0.120;
    if (product_id.find("30Y") != std::string::npos) return 0.150;
    return 0.050;
  }

  std::map<std::string, PV01<Bond>> risks_;
  std::vector<ServiceListener<PV01<Bond>>*> listeners_;
  mutable PV01<BucketedSector<Bond>> cached_bucket_ =
      PV01<BucketedSector<Bond>>(BucketedSector<Bond>({}, "EMPTY"), 0.0, 0);
};

#endif
