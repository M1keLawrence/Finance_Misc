#include <iostream>
#include <string>
#include "MarketDataFileToShmPublisher.hpp"

int main(int argc, char** argv) {
  std::string file = (argc > 1) ? argv[1] : "marketdata.txt";
  std::string shm  = (argc > 2) ? argv[2] : "BOND_MD_SHM";
  MarketDataFileToShmProcess(file, shm);
  std::cout << "Published marketdata file to SHM: " << shm << "\n";
  return 0;
}
