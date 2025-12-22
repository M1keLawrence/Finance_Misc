/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * NOTE: Your earlier uploaded copy contained literal "..." placeholders.
 * This is a reconstructed, complete header compatible with the course framework.
 */
#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include <string>
#include <vector>

#include "soa.hpp"
#include "positionservice.hpp"

using std::string;
using std::vector;

/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01
{
public:
  // ctor for a PV01 value
  PV01(const T &_product, double _pv01, long _quantity)
      : product(_product), pv01(_pv01), quantity(_quantity) {}

  // Get the product on this PV01 value
  const T& GetProduct() const { return product; }

  // Get the PV01 value (typically PV01-per-unit; interpretation is up to service)
  double GetPV01() const { return pv01; }

  // Get the quantity that this risk value is associated with
  long GetQuantity() const { return quantity; }

private:
  T product;
  double pv01;
  long quantity;
};

/**
 * A bucket sector to bucket a group of securities.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{
public:
  BucketedSector(const vector<T> &_products, const string &_name)
      : products(_products), name(_name) {}

  // Get the products in this bucket
  const vector<T>& GetProducts() const { return products; }

  // Get the name of the bucket
  const string& GetName() const { return name; }

private:
  vector<T> products;
  string name;
};

/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string, PV01<T>>
{
public:
  // Update risk given a position
  virtual void AddPosition(Position<T> &position) = 0;

  // Get bucketed risk
  virtual const PV01<BucketedSector<T>>& GetBucketedRisk(
      const BucketedSector<T>& sector) const = 0;
};

#endif
