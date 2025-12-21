#ifndef BOND_TYPED_CONNECTORS_HPP
#define BOND_TYPED_CONNECTORS_HPP

#include "BondSocketConnectors.hpp"
#include "BondSocketParsers.hpp"

// Pricing inbound (socket -> BondPricingService::OnMessage)
template <typename PricingServiceT>
inline TcpInboundConnector<Price<Bond>, PricingServiceT>
MakePricingInbound(PricingServiceT& svc, int port) {
  return TcpInboundConnector<Price<Bond>, PricingServiceT>(svc, port, ParsePriceLine);
}

// Trades inbound (socket -> BondTradeBookingService::OnMessage)
template <typename TradeBookingServiceT>
inline TcpInboundConnector<Trade<Bond>, TradeBookingServiceT>
MakeTradesInbound(TradeBookingServiceT& svc, int port) {
  return TcpInboundConnector<Trade<Bond>, TradeBookingServiceT>(svc, port, ParseTradeLine);
}

// Inquiries inbound (socket -> BondInquiryService::OnMessage)
template <typename InquiryServiceT>
inline TcpInboundConnector<Inquiry<Bond>, InquiryServiceT>
MakeInquiriesInbound(InquiryServiceT& svc, int port) {
  return TcpInboundConnector<Inquiry<Bond>, InquiryServiceT>(svc, port, ParseInquiryLine);
}

// Execution outbound (BondExecutionService publishes -> socket)
inline TcpOutboundConnector<ExecutionOrder<Bond>>
MakeExecutionOutbound(const std::string& host, int port) {
  return TcpOutboundConnector<ExecutionOrder<Bond>>(host, port, SerializeExecution);
}

// Streaming outbound (BondStreamingService publishes -> socket)
inline TcpOutboundConnector<PriceStream<Bond>>
MakeStreamingOutbound(const std::string& host, int port) {
  return TcpOutboundConnector<PriceStream<Bond>>(host, port, SerializePriceStream);
}

// Inquiry outbound (BondInquiryService sends quote/state updates -> socket)
inline TcpOutboundConnector<Inquiry<Bond>>
MakeInquiryOutbound(const std::string& host, int port) {
  return TcpOutboundConnector<Inquiry<Bond>>(host, port, SerializeInquiry);
}

#endif
