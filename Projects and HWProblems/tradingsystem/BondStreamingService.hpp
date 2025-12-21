#ifndef BOND_STREAMING_SERVICE_HPP
#define BOND_STREAMING_SERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include "products.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"

class BondStreamingService final : public StreamingService<Bond> {
public:
    BondStreamingService() = default;

    void SetPublishConnector(Connector<PriceStream<Bond>>* connector) 
    {
        pub_connector_ = connector;
    }

    PriceStream<Bond>& GetData(std::string key) override { return streams_.at(key); }

    void OnMessage(PriceStream<Bond>&) override 
    {
        // No inbound connector per spec.
    }

    void AddListener(ServiceListener<PriceStream<Bond>>* listener) override 
    {
        listeners_.push_back(listener);
    }

    const std::vector<ServiceListener<PriceStream<Bond>>*>& GetListeners() const override 
    {
        return listeners_;
    }

    void PublishPrice(const PriceStream<Bond>& priceStream) override 
    {
        const std::string pid = priceStream.GetProduct().GetProductId();
        streams_.erase(pid);
        streams_.emplace(pid, PriceStream<Bond>(priceStream.GetProduct(),
                                            priceStream.GetBidOrder(),
                                            priceStream.GetOfferOrder()));


        PriceStream<Bond>& stored = streams_.at(pid);
        for (auto* l : listeners_) l->ProcessAdd(stored);

        if (pub_connector_) pub_connector_->Publish(stored);
    }

private:
    std::map<std::string, PriceStream<Bond>> streams_;
    std::vector<ServiceListener<PriceStream<Bond>>*> listeners_;
    Connector<PriceStream<Bond>>* pub_connector_ = nullptr;
};

#endif
