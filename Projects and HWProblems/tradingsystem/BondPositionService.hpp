#ifndef BOND_POSITION_SERVICE_HPP
#define BOND_POSITION_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "positionservice.hpp"
#include "products.hpp"    // Bond + BucketNameForProduct
#include "riskservice.hpp" // BucketedSector
#include "soa.hpp"
#include "tradebookingservice.hpp"

/**
 * BondPositionService
 *
 * Maintains per-security Position<Bond> keyed by product id.
 * Also provides a helper to aggregate positions into a bucketed sector
 * (FrontEnd / Belly / LongEnd) via BucketNameForProduct(product_id).
 */
class BondPositionService final : public PositionService<Bond>,
                                 public ServiceListener<Trade<Bond>> {
public:
  BondPositionService() = default;

  // Service<string, Position<Bond>>
  Position<Bond>& GetData(std::string key) override { return positions_.at(key); }

  void OnMessage(Position<Bond>& data) override {
    const std::string pid = data.GetProduct().GetProductId();
    positions_.insert_or_assign(pid, data);
    for (auto* l : listeners_) {
      if (l) l->ProcessUpdate(positions_.at(pid));
    }
  }

  void AddListener(ServiceListener<Position<Bond>>* listener) override {
    listeners_.push_back(listener);
  }

  const std::vector<ServiceListener<Position<Bond>>*>& GetListeners() const override {
    return listeners_;
  }

  // PositionService<Bond>
  void AddTrade(const Trade<Bond>& trade) override {
    const std::string pid = trade.GetProduct().GetProductId();

    auto it = positions_.find(pid);
    if (it == positions_.end()) {
      it = positions_.emplace(pid, Position<Bond>(trade.GetProduct())).first;
    }

    const long signed_qty = (trade.GetSide() == BUY) ? trade.GetQuantity() : -trade.GetQuantity();
    it->second.AddPosition(trade.GetBook(), signed_qty);

    for (auto* l : listeners_) {
      if (l) l->ProcessUpdate(it->second);
    }
  }

  /**
   * Aggregate per-security positions into a bucket-sector Position.
   * Returned object has a single book "AGG" containing the aggregate.
   */
  Position<BucketedSector<Bond>> GetBucketedPosition(const BucketedSector<Bond>& sector) const {
    long qty_sum = 0;
    for (const auto& kv : positions_) {
      if (BucketNameForProduct(kv.first) != sector.GetName()) continue;
      qty_sum += kv.second.GetAggregatePosition();
    }

    Position<BucketedSector<Bond>> bucket_pos(sector);
    bucket_pos.SetPosition("AGG", qty_sum);
    return bucket_pos;
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
