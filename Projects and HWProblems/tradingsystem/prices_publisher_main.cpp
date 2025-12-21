#include <iostream>
#include <string>
#include "ExternalFileToSocketPublishers.hpp"

int main(int argc, char** argv) {
  // Usage: ./prices_publisher [file] [host] [port]
  std::string file = "prices.txt";
  std::string host = "127.0.0.1";
  int port = 9001;

  if (argc > 1) file = argv[1];
  if (argc > 2) host = argv[2];
  if (argc > 3) port = std::stoi(argv[3]);

  try {
    PricesFileProcess(file, host, port);
  } catch (const std::exception& e) {
    std::cerr << "prices_publisher error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}