#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: stream_print <port>\n";
    return 1;
  }

  int port = std::atoi(argv[1]);

  try {
    boost::asio::io_context io;

    // Server socket: listen for trading_system to connect
    boost::asio::ip::tcp::acceptor acceptor(
        io,
        boost::asio::ip::tcp::endpoint(
            boost::asio::ip::tcp::v4(),
            static_cast<unsigned short>(port)));

    boost::asio::ip::tcp::socket socket(io);
    acceptor.accept(socket);

    boost::asio::streambuf buffer;

    while (true) {
      boost::system::error_code ec;
      boost::asio::read_until(socket, buffer, '\n', ec);

      // Normal shutdown: trading_system closed the socket
      if (ec == boost::asio::error::eof) {
        std::cerr << "stream_print: connection closed by peer (EOF). Exiting.\n";
        break;
      }

      if (ec) {
        throw boost::system::system_error(ec);
      }

      std::istream is(&buffer);
      std::string line;
      std::getline(is, line);

      // Handle CRLF
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      if (!line.empty()) {
        std::cout << "[STREAM] " << line << std::endl;
      }
    }
  }
  catch (const std::exception& e) {
    std::cerr << "stream_print error: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}
