/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include "soa.hpp"
#include "tradebookingservice.hpp"

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:

  // ctor for a position
  Position(const T &_product);

  // Get the product
  const T& GetProduct() const;

  // Get the position quantity
  long GetPosition(const string &book) const;

  // Add (signed) quantity to a book
  void AddPosition(const string &book, long quantity);

  // Set quantity for a book
  void SetPosition(const string &book, long quantity);

  // Get all book positions
  const map<string,long>& GetPositions() const;

  // Get the aggregate position
  long GetAggregatePosition() const;

private:
  T product;
  map<string,long> positions;

};

/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string,Position <T> >
{

public:

  // Add a trade to the service
  virtual void AddTrade(const Trade<T> &trade) = 0;

};

template<typename T>
Position<T>::Position(const T &_product) :
  product(_product)
{
}

template<typename T>
const T& Position<T>::GetProduct() const
{
  return product;
}

template<typename T>
long Position<T>::GetPosition(const string &book) const
{
  auto it = positions.find(book);
  return (it == positions.end()) ? 0L : it->second;
}

template<typename T>
void Position<T>::AddPosition(const string &book, long quantity)
{
  positions[book] += quantity;
}

template<typename T>
void Position<T>::SetPosition(const string &book, long quantity)
{
  positions[book] = quantity;
}

template<typename T>
const map<string,long>& Position<T>::GetPositions() const
{
  return positions;
}

template<typename T>
long Position<T>::GetAggregatePosition() const
{
  long sum = 0;
  for (const auto& kv : positions) sum += kv.second;
  return sum;
}

#endif
