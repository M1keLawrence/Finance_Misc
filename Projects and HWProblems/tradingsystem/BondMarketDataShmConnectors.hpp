#ifndef BOND_MARKETDATA_SHM_CONNECTORS_HPP
#define BOND_MARKETDATA_SHM_CONNECTORS_HPP

#include <functional>
#include <string>

#include "BondSocketParsers.hpp"
#include "ShmStringRingBuffer.hpp"
#include "soa.hpp"

// Recommended queue sizing for high-throughput
constexpr std::size_t kMdShmCapacity = 8192;
constexpr std::size_t kMdMsgSize = 2048;

// --------- Publisher: turns OrderBook into string and pushes to SHM ----------
class BondMarketDataShmPublisher : public Connector<OrderBook<Bond>> {
 public:
  explicit BondMarketDataShmPublisher(const std::string& shm_name)
      : shm_(shm_name, /*create=*/true) {}

  void Publish(OrderBook<Bond>& ob) override {
    shm_.Push(SerializeOrderBook(ob));
  }

 private:
  ShmQueueHandle<kMdShmCapacity, kMdMsgSize> shm_;
};

// --------- Subscriber: pops string from SHM, parses, calls Service.OnMessage ----------
template <typename MarketDataServiceT>
class BondMarketDataShmSubscriber : public Connector<OrderBook<Bond>> {
 public:
  BondMarketDataShmSubscriber(MarketDataServiceT& svc, const std::string& shm_name)
      : service_(svc), shm_(shm_name, /*create=*/false) {}

  void Subscribe() {
    while (true) {
      std::string msg = shm_.Pop();
      OrderBook<Bond> ob = ParseOrderBookLine(msg);
      service_.OnMessage(ob);
    }
  }

  void Publish(OrderBook<Bond>&) override {
    // subscribe-only
  }

 private:
  MarketDataServiceT& service_;
  ShmQueueHandle<kMdShmCapacity, kMdMsgSize> shm_;
};

#endif
