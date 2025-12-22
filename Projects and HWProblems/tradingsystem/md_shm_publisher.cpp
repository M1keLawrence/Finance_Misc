#include <iostream>
#include <string>

#include "MarketDataFileToShmPublisher.hpp"

int main(int argc, char** argv) {
  std::string file = "marketdata.txt";
  std::string shm  = "BOND_MD_SHM";
  boost::interprocess::shared_memory_object::remove("BOND_MD_SHM");

  if (argc > 1) file = argv[1];
  if (argc > 2) shm  = argv[2];

  try {
    MarketDataFileToShmProcess(file, shm);
    std::cout << "Published market data from " << file << " to SHM " << shm << "\n";
  } catch (const std::exception& e) {
    std::cerr << "md_shm_publisher error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
