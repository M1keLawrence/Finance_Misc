#ifndef HISTORICAL_FILE_CONNECTORS_HPP
#define HISTORICAL_FILE_CONNECTORS_HPP

#include <chrono>
#include <fstream>
#include <string>

#include "BondPriceUtils.hpp"
#include "BondSocketParsers.hpp"
#include "soa.hpp"

inline long long EpochMillisNow() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

template <typename V>
class FilePublishConnector : public Connector<V> {
 public:
  explicit FilePublishConnector(const std::string& filename,
                               std::function<std::string(const V&)> serializer)
      : out_(filename, std::ios::out), serializer_(std::move(serializer)) {}

  void Publish(V& data) override {
    out_ << EpochMillisNow() << "," << serializer_(data) << "\n";
    out_.flush();
  }

 private:
  std::ofstream out_;
  std::function<std::string(const V&)> serializer_;
};

#endif
