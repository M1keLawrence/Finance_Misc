#ifndef BOND_SOCKET_CONNECTORS_HPP
#define BOND_SOCKET_CONNECTORS_HPP

#include <boost/asio.hpp>
#include <functional>
#include <string>

#include "TcpLineSocket.hpp"
#include "soa.hpp"

// -------- Inbound (subscriber) connector pattern --------
// NOTE: Connector<V> base only requires Publish(V&).
// We add Subscribe() as a concrete method you call on the derived type.

template <typename V, typename ServiceT>
class TcpInboundConnector : public Connector<V> {
 public:
  TcpInboundConnector(ServiceT& service,
                      int listen_port,
                      std::function<V(const std::string&)> parser)
      : service_(service), port_(listen_port), parser_(std::move(parser)) {}

  // Subscriber loop (blocking). Accepts one client connection.
  void Subscribe() {
    boost::asio::io_context io;
    TcpLineServer server(io, port_);

    for (;;) {                       // accept-reaccept loop
      server.AcceptOne();

      while (true) {                 // read loop for that connection
        auto lineOpt = server.ReadLine();
        if (!lineOpt) break;         // peer closed => go back to AcceptOne()

        if (lineOpt->empty()) continue;

        try {
          V obj = parser_(*lineOpt);
          service_.OnMessage(obj);
        } catch (const std::exception& e) {
          std::cerr << "[InboundConnector] parse error: " << e.what()
                    << " | line='" << *lineOpt << "'\n";
        }
      }
    }
  }

void Publish(V&) override {
    // subscribe-only
  }

 private:
  ServiceT& service_;
  int port_;
  std::function<V(const std::string&)> parser_;
};

// -------- Outbound (publisher) connector pattern --------
template <typename V>
class TcpOutboundConnector : public Connector<V> {
 public:
  TcpOutboundConnector(const std::string& host, int port,
                       std::function<std::string(const V&)> serializer)
      : host_(host), port_(port), serializer_(std::move(serializer)) {}

  void Connect() {
    io_ = std::make_unique<boost::asio::io_context>();
    client_ = std::make_unique<TcpLineClient>(*io_);
    client_->Connect(host_, port_);
  }

  void Publish(V& data) override {
    if (!client_) Connect();
    client_->WriteLine(serializer_(data));
  }

 private:
  std::string host_;
  int port_;
  std::function<std::string(const V&)> serializer_;
  std::unique_ptr<boost::asio::io_context> io_;
  std::unique_ptr<TcpLineClient> client_;
};

#endif
