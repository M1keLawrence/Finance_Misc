#ifndef MARKETDATA_FILE_TO_SHM_PUBLISHER_HPP
#define MARKETDATA_FILE_TO_SHM_PUBLISHER_HPP

#include <fstream>
#include <string>

#include "BondMarketDataShmConnectors.hpp"
#include "BondSocketParsers.hpp"
#include "BondUniverse.hpp"
#include <boost/interprocess/shared_memory_object.hpp>


inline void MarketDataFileToShmProcess(const std::string& marketdata_file,
                                       const std::string& shm_name) {
  RegisterBondUniverse();

  std::ifstream in(marketdata_file);
  if (!in) throw std::runtime_error("Cannot open market data file: " + marketdata_file);

  // Create SHM segment and publish messages
  boost::interprocess::shared_memory_object::remove("BOND_MD_SHM");
  ShmQueueHandle<kMdShmCapacity, kMdMsgSize> shm(shm_name, /*create=*/true);

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    // Validate parse once (optional) and push raw string
    (void)ParseOrderBookLine(line);
    shm.Push(line);
  }
}

#endif
