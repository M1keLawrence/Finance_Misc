#ifndef BOND_PRODUCT_REPOSITORY_HPP
#define BOND_PRODUCT_REPOSITORY_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "products.hpp"

class BondProductRepository {
 public:
  static BondProductRepository& Instance() {
    static BondProductRepository inst;
    return inst;
  }

  // Register bond metadata (call once at startup in each process).
  void Register(const std::string& product_id,
                BondIdType id_type,
                const std::string& ticker,
                float coupon,
                const boost::gregorian::date& maturity) {
    bonds_.emplace(product_id, Bond(product_id, id_type, ticker, coupon, maturity));
  }

  // Get stable reference. Throws if missing.
  const Bond& Get(const std::string& product_id) const {
    auto it = bonds_.find(product_id);
    if (it == bonds_.end()) throw std::runtime_error("Unknown Bond product_id: " + product_id);
    return it->second;
  }

 private:
  BondProductRepository() = default;
  std::map<std::string, Bond> bonds_;
};

#endif