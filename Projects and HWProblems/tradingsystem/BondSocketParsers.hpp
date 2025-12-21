#ifndef BOND_SOCKET_PARSERS_HPP
#define BOND_SOCKET_PARSERS_HPP

#include <string>
#include <vector>

#include "BondPriceUtils.hpp"
#include "BondProductRepository.hpp"
#include "CsvUtils.hpp"
#include "executionservice.hpp"
#include "inquiryservice.hpp"
#include "marketdataservice.hpp"
#include "pricingservice.hpp"
#include "streamingservice.hpp"
#include "tradebookingservice.hpp"

// ---------- Price<Bond> ----------
inline Price<Bond> ParsePriceLine(const std::string& line) {
  // productId,mid,spread
  auto f = Split(line, ',');
  if (f.size() != 3) throw std::runtime_error("Bad price line: " + line);
  const Bond& b = BondProductRepository::Instance().Get(f[0]);
  double mid = ParsePriceMaybeFractional(f[1]);
  double spr = ParsePriceMaybeFractional(f[2]);  // allow fractional for spread too
  return Price<Bond>(b, mid, spr);
}

inline std::string SerializePrice(const Price<Bond>& p) {
  // productId,mid,spread (fractional for mid; decimal spread is ok; we do fractional too)
  return p.GetProduct().GetProductId() + "," + FormatPriceFractional(p.GetMid()) + "," +
         FormatPriceFractional(p.GetBidOfferSpread());
}

// ---------- Trade<Bond> ----------
inline Trade<Bond> ParseTradeLine(const std::string& line) {
  // tradeId,productId,price,book,qty,side
  auto f = Split(line, ',');
  if (f.size() != 6) throw std::runtime_error("Bad trade line: " + line);

  const Bond& b = BondProductRepository::Instance().Get(f[1]);
  double px = ParsePriceMaybeFractional(f[2]);
  long qty = std::stol(f[4]);
  Side side = (f[5] == "BUY") ? BUY : SELL;
  return Trade<Bond>(b, f[0], px, f[3], qty, side);
}

inline std::string SerializeTrade(const Trade<Bond>& t) {
  return t.GetTradeId() + "," + t.GetProduct().GetProductId() + "," + FormatPriceFractional(t.GetPrice()) + "," +
         t.GetBook() + "," + std::to_string(t.GetQuantity()) + "," + (t.GetSide() == BUY ? "BUY" : "SELL");
}

// ---------- OrderBook<Bond> (market data) ----------
inline OrderBook<Bond> ParseOrderBookLine(const std::string& line) {
  // productId|bidPx:qty;bidPx:qty;...|offerPx:qty;offerPx:qty;...
  auto parts = Split(line, '|');
  if (parts.size() != 3) throw std::runtime_error("Bad orderbook line: " + line);

  const Bond& b = BondProductRepository::Instance().Get(parts[0]);

  auto parse_stack = [&](const std::string& s, PricingSide side) {
    std::vector<Order> out;
    if (s.empty()) return out;
    auto lvls = Split(s, ';');
    for (const auto& lvl : lvls) {
      if (lvl.empty()) continue;
      auto pq = Split(lvl, ':');
      if (pq.size() != 2) throw std::runtime_error("Bad level: " + lvl);
      double px = ParsePriceMaybeFractional(pq[0]);
      long qty = std::stol(pq[1]);
      out.emplace_back(px, qty, side);
    }
    return out;
  };

  std::vector<Order> bids = parse_stack(parts[1], BID);
  std::vector<Order> offers = parse_stack(parts[2], OFFER);
  return OrderBook<Bond>(b, bids, offers);
}

inline std::string SerializeOrderBook(const OrderBook<Bond>& ob) {
  auto stack_to_str = [&](const std::vector<Order>& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
      if (i) out += ";";
      out += FormatPriceFractional(s[i].GetPrice());
      out += ":";
      out += std::to_string(s[i].GetQuantity());
    }
    return out;
  };

  return ob.GetProduct().GetProductId() + "|" + stack_to_str(ob.GetBidStack()) + "|" + stack_to_str(ob.GetOfferStack());
}

// ---------- ExecutionOrder<Bond> ----------
inline std::string SerializeExecution(const ExecutionOrder<Bond>& e) {
  // productId,orderId,ordertype,price,visible,hidden,parent,isChild
  // (side omitted since base class doesn’t expose GetSide)
  return e.GetProduct().GetProductId() + "," + e.GetOrderId() + "," + std::to_string(static_cast<int>(e.GetOrderType())) +
         "," + FormatPriceFractional(e.GetPrice()) + "," + std::to_string(e.GetVisibleQuantity()) + "," +
         std::to_string(e.GetHiddenQuantity()) + "," + e.GetParentOrderId() + "," + (e.IsChildOrder() ? "1" : "0");
}

// ---------- PriceStream<Bond> ----------
inline std::string SerializePriceStream(const PriceStream<Bond>& ps) {
  // productId,bidPx,bidVis,bidHid,offerPx,offerVis,offerHid
  const auto& bid = ps.GetBidOrder();
  const auto& off = ps.GetOfferOrder();
  return ps.GetProduct().GetProductId() + "," +
         FormatPriceFractional(bid.GetPrice()) + "," + std::to_string(bid.GetVisibleQuantity()) + "," +
         std::to_string(bid.GetHiddenQuantity()) + "," +
         FormatPriceFractional(off.GetPrice()) + "," + std::to_string(off.GetVisibleQuantity()) + "," +
         std::to_string(off.GetHiddenQuantity());
}

// ---------- Inquiry<Bond> ----------
inline Inquiry<Bond> ParseInquiryLine(const std::string& line) {
  // inquiryId,productId,side,qty,price,state
  auto f = Split(line, ',');
  if (f.size() != 6) throw std::runtime_error("Bad inquiry line: " + line);

  const Bond& b = BondProductRepository::Instance().Get(f[1]);
  Side side = (f[2] == "BUY") ? BUY : SELL;
  long qty = std::stol(f[3]);
  double px = ParsePriceMaybeFractional(f[4]);

  InquiryState st = RECEIVED;
  if (f[5] == "RECEIVED") st = RECEIVED;
  else if (f[5] == "QUOTED") st = QUOTED;
  else if (f[5] == "DONE") st = DONE;
  else if (f[5] == "REJECTED") st = REJECTED;
  else if (f[5] == "CUSTOMER_REJECTED") st = CUSTOMER_REJECTED;

  return Inquiry<Bond>(f[0], b, side, qty, px, st);
}

inline std::string SerializeInquiry(const Inquiry<Bond>& i) {
  auto st = [&](InquiryState s) {
    switch (s) {
      case RECEIVED: return "RECEIVED";
      case QUOTED: return "QUOTED";
      case DONE: return "DONE";
      case REJECTED: return "REJECTED";
      case CUSTOMER_REJECTED: return "CUSTOMER_REJECTED";
    }
    return "RECEIVED";
  };

  return i.GetInquiryId() + "," + i.GetProduct().GetProductId() + "," +
         (i.GetSide() == BUY ? "BUY" : "SELL") + "," + std::to_string(i.GetQuantity()) + "," +
         FormatPriceFractional(i.GetPrice()) + "," + st(i.GetState());
}

#endif
