#ifndef BOND_UNIVERSE_HPP
#define BOND_UNIVERSE_HPP

#include "BondProductRepository.hpp"

// Replace these placeholders with real values (CUSIPs/coupons/maturities).
inline void RegisterBondUniverse() {
  auto& repo = BondProductRepository::Instance();

  // product_id, id_type, ticker, coupon, maturity
  repo.Register("2Y",  CUSIP, "T", 0.045f, boost::gregorian::date(2027, 12, 31));
  repo.Register("3Y",  CUSIP, "T", 0.045f, boost::gregorian::date(2028, 12, 31));
  repo.Register("5Y",  CUSIP, "T", 0.045f, boost::gregorian::date(2030, 12, 31));
  repo.Register("7Y",  CUSIP, "T", 0.045f, boost::gregorian::date(2032, 12, 31));
  repo.Register("10Y", CUSIP, "T", 0.045f, boost::gregorian::date(2035, 12, 31));
  repo.Register("20Y", CUSIP, "T", 0.045f, boost::gregorian::date(2045, 12, 31));
  repo.Register("30Y", CUSIP, "T", 0.045f, boost::gregorian::date(2055, 12, 31));
}

#endif
