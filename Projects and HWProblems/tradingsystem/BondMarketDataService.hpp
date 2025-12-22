#ifndef BOND_MARKET_DATA_SERVICE_HPP
#define BOND_MARKET_DATA_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "marketdataservice.hpp"
#include "products.hpp"
#include "soa.hpp"

/**
 * BondMarketDataService
 * Stores full order books keyed by product id and maintains best bid/offer.
 */
class BondMarketDataService final : public MarketDataService<Bond> {
public:
  BondMarketDataService() = default;

  // Service<string, OrderBook<Bond>>
  OrderBook<Bond>& GetData(std::string key) override { return books_.at(key); }

  void OnMessage(OrderBook<Bond>& data) override {
    const std::string pid = data.GetProduct().GetProductId();
    books_.insert_or_assign(pid, data);

    // Update best bid/offer cache
    const auto& bids = books_.at(pid).GetBidStack();
    const auto& offers = books_.at(pid).GetOfferStack();
    if (!bids.empty() && !offers.empty()) {
      best_.insert_or_assign(pid, BidOffer(bids.front(), offers.front()));
    }

    for (auto* l : listeners_) {
      if (l) l->ProcessUpdate(books_.at(pid));
    }
  }

  void AddListener(ServiceListener<OrderBook<Bond>>* listener) override {
    listeners_.push_back(listener);
  }

  const std::vector<ServiceListener<OrderBook<Bond>>*>& GetListeners() const override {
    return listeners_;
  }

  // MarketDataService<Bond>
  const BidOffer& GetBestBidOffer(const std::string& productId) override {
    return best_.at(productId);
  }

  const OrderBook<Bond>& AggregateDepth(const std::string& productId) override {
    return books_.at(productId);
  }

private:
  std::map<std::string, OrderBook<Bond>> books_;
  std::map<std::string, BidOffer> best_;
  std::vector<ServiceListener<OrderBook<Bond>>*> listeners_;
};

#endif
