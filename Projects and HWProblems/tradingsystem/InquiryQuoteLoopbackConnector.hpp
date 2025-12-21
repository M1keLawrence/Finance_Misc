#ifndef INQUIRY_QUOTE_LOOPBACK_CONNECTOR_HPP
#define INQUIRY_QUOTE_LOOPBACK_CONNECTOR_HPP

#include "inquiryservice.hpp"
#include "soa.hpp"

// A connector that loops quote updates back into the InquiryService via OnMessage.
// Publish() receives a quote request (price set to 100 by the service).
// It sends QUOTED then DONE back into the service.
template <typename InquiryServiceT, typename ProductT>
class InquiryQuoteLoopbackConnector final : public Connector<Inquiry<ProductT>> {
 public:
  explicit InquiryQuoteLoopbackConnector(InquiryServiceT& svc) : svc_(svc) {}

  void Publish(Inquiry<ProductT>& req) override {
    // Transition to QUOTED with supplied price
    Inquiry<ProductT> quoted(req.GetInquiryId(), req.GetProduct(), req.GetSide(),
                            req.GetQuantity(), req.GetPrice(), QUOTED);
    svc_.OnMessage(quoted);

    // Immediately transition to DONE
    Inquiry<ProductT> done(req.GetInquiryId(), req.GetProduct(), req.GetSide(),
                          req.GetQuantity(), req.GetPrice(), DONE);
    svc_.OnMessage(done);
  }

 private:
  InquiryServiceT& svc_;
};

#endif