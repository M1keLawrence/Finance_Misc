#ifndef GUI_SERVICE_HPP
#define GUI_SERVICE_HPP

#include <chrono>
#include <fstream>
#include <string>

#include "pricingservice.hpp"
#include "products.hpp"
#include "soa.hpp"

class GUIService final : public ServiceListener<Price<Bond>> {
public:
    explicit GUIService(const std::string& out_file = "gui.txt",
                        std::chrono::milliseconds throttle = std::chrono::milliseconds(300))
        : out_(out_file, std::ios::out), throttle_(throttle) 
    {
        last_emit_ = std::chrono::steady_clock::now();
    }

    void ProcessAdd(Price<Bond>& p) override { ProcessUpdate(p); }
    void ProcessRemove(Price<Bond>&) override {}

    void ProcessUpdate(Price<Bond>& p) override 
    {
        if (printed_ >= 100) return;

        const auto now = std::chrono::steady_clock::now();
        if (now - last_emit_ < throttle_) return;

        last_emit_ = now;
        ++printed_;

        const auto ts_ms = NowEpochMillis();

        out_ << ts_ms << "," << p.GetProduct().GetProductId()
                << "," << p.GetMid()
                << "," << p.GetBidOfferSpread()
                << "\n";
        out_.flush();
    }

private:
    static long long NowEpochMillis() 
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    std::ofstream out_;
    std::chrono::milliseconds throttle_;
    std::chrono::steady_clock::time_point last_emit_;
    int printed_ = 0;
};

#endif
