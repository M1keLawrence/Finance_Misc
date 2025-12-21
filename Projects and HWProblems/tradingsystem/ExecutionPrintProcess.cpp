#include <iostream>
#include <string>

#include "TcpLineSocket.hpp"

int main(int argc, char** argv) {
  int port = 9101;
  if (argc > 1) port = std::stoi(argv[1]);

  boost::asio::io_context io;
  TcpLineServer server(io, port);
  server.AcceptOne();

  try 
  {
    while (true) 
    {
      auto lineOpt = server.ReadLine();     // std::optional<std::string>
      if (!lineOpt) {
        std::cerr << "[EXEC] peer closed connection (EOF). Exiting.\n";
        break;
      }
      const std::string& line = *lineOpt;
      if (!line.empty())
        std::cout << "[EXEC] " << line << "\n";
    }
  } catch (const boost::system::system_error& e) {
    if (e.code() == boost::asio::error::eof) {
      std::cerr << "[EXEC] peer closed connection\n";
      return 0;
    }
    throw; // unexpected asio error
  }
}
