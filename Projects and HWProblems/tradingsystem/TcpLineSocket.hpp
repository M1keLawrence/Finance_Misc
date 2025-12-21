#ifndef TCP_LINE_SOCKET_HPP
#define TCP_LINE_SOCKET_HPP

#include <boost/asio.hpp>
#include <string>

class TcpLineServer {
 public:
  TcpLineServer(boost::asio::io_context& io, int port)
      : acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        socket_(io) {}

  void AcceptOne() {
    // if you re-accept after a disconnect, ensure the socket is closed first
    if (socket_.is_open()) socket_.close();
    acceptor_.accept(socket_);
  }

  std::optional<std::string> ReadLine() {
    boost::system::error_code ec;
    boost::asio::read_until(socket_, buf_, "\n", ec);

    if (ec == boost::asio::error::eof) {
      // peer closed; close our side so AcceptOne can be called again safely
      socket_.close();
      return std::nullopt;
    }
    if (ec) throw boost::system::system_error(ec);

    std::istream is(&buf_);
    std::string line;
    std::getline(is, line);

    if (!line.empty() && line.back() == '\r') line.pop_back();
    return line;
  }

 private:
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf buf_;
};


class TcpLineClient {
 public:
  explicit TcpLineClient(boost::asio::io_context& io) : socket_(io) {}

  void Connect(const std::string& host, int port) {
    boost::asio::ip::tcp::resolver resolver(socket_.get_executor());
    auto endpoints = resolver.resolve(host, std::to_string(port));
    boost::asio::connect(socket_, endpoints);
  }

  void WriteLine(const std::string& line) {
    const std::string msg = line + "\n";
    boost::asio::write(socket_, boost::asio::buffer(msg));
  }

 private:
  boost::asio::ip::tcp::socket socket_;
};

#endif
