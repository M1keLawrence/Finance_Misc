#ifndef EXTERNAL_FILE_TO_SOCKET_PUBLISHERS_HPP
#define EXTERNAL_FILE_TO_SOCKET_PUBLISHERS_HPP

#include <fstream>
#include <string>

#include "BondSocketParsers.hpp"
#include "TcpLineSocket.hpp"

inline void PublishFileLinesToSocket(const std::string& filename,
                                     const std::string& host, int port) {
  std::ifstream in(filename);
  if (!in) throw std::runtime_error("Cannot open file: " + filename);

  boost::asio::io_context io;
  TcpLineClient client(io);
  client.Connect(host, port);

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    client.WriteLine(line);
  }
}

// Dedicated wrappers (optional clarity)
inline void PricesFileProcess(const std::string& filename, const std::string& host, int port) {
  PublishFileLinesToSocket(filename, host, port);
}
inline void TradesFileProcess(const std::string& filename, const std::string& host, int port) {
  PublishFileLinesToSocket(filename, host, port);
}
inline void InquiriesFileProcess(const std::string& filename, const std::string& host, int port) {
  PublishFileLinesToSocket(filename, host, port);
}

#endif