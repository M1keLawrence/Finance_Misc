#ifndef BOND_INQUIRY_SERVICE_HPP
#define BOND_INQUIRY_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "inquiryservice.hpp"
#include "products.hpp"
#include "soa.hpp"

class BondInquiryService final : public InquiryService<Bond> {
 public:
  BondInquiryService() = default;

  void SetConnector(Connector<Inquiry<Bond>>* connector) { connector_ = connector; }

  Inquiry<Bond>& GetData(std::string key) override { return inquiries_.at(key); }

  void OnMessage(Inquiry<Bond>& data) override {
    const std::string id = data.GetInquiryId();
    inquiries_.erase(id);
    inquiries_.emplace(id, Inquiry<Bond>(data.GetInquiryId(),
                                        data.GetProduct(),
                                        data.GetSide(),
                                        data.GetQuantity(),
                                        data.GetPrice(),
                                        data.GetState()));


    Inquiry<Bond>& stored = inquiries_.at(id);
    for (auto* l : listeners_) l->ProcessUpdate(stored);

    // If RECEIVED, request quote workflow via Connector.Publish() (spec).
    if (stored.GetState() == RECEIVED && connector_) {
      // Publish a quote request with price = 100.0 (spec says quote of 100)
      Inquiry<Bond> req(stored.GetInquiryId(), stored.GetProduct(), stored.GetSide(),
                        stored.GetQuantity(), 100.0, RECEIVED);
      connector_->Publish(req);
    }
  }

  void AddListener(ServiceListener<Inquiry<Bond>>* listener) override { listeners_.push_back(listener); }

  const std::vector<ServiceListener<Inquiry<Bond>>*>& GetListeners() const override { return listeners_; }

  void SendQuote(const std::string& inquiryId, double price) override {
    // Not used directly in this year’s spec flow (connector handles QUOTED and DONE)
    (void)inquiryId;
    (void)price;
  }

  void RejectInquiry(const std::string& inquiryId) override {
    // Optional: could publish REJECTED via connector; not required for the core spec.
    (void)inquiryId;
  }

 private:
  std::map<std::string, Inquiry<Bond>> inquiries_;
  std::vector<ServiceListener<Inquiry<Bond>>*> listeners_;
  Connector<Inquiry<Bond>>* connector_ = nullptr;
};

#endif
