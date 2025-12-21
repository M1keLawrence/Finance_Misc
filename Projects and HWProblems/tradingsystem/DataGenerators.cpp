#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "BondPriceUtils.hpp"

static std::vector<std::string> Products() {
  return {"2Y","3Y","5Y","7Y","10Y","20Y","30Y"};
}

static double Tick256() { return 1.0 / 256.0; }

static void GenPrices(const std::string& file, long N_per_product) {
  std::ofstream out(file);
  auto prods = Products();

  // Oscillate mid between 99 and 101 by 1/256.
  // Spread oscillates between 1/128 and 1/64.
  for (const auto& pid : prods) {
    double mid = 99.0;
    bool up = true;
    double spr = 1.0/128.0;
    bool spr_up = true;

    for (long i=0; i<N_per_product; ++i) {
      out << pid << "," << FormatPriceFractional(mid) << "," << FormatPriceFractional(spr) << "\n";

      // mid move
      mid += up ? Tick256() : -Tick256();
      if (mid >= 101.0) up = false;
      if (mid <= 99.0) up = true;

      // spread move between 1/128 and 1/64 in smallest increments (use 1/256 steps)
      spr += spr_up ? Tick256() : -Tick256();
      if (spr >= 1.0/64.0) spr_up = false;
      if (spr <= 1.0/128.0) spr_up = true;
    }
  }
}

static void GenTrades(const std::string& file) {
  std::ofstream out(file);
  auto prods = Products();
  std::vector<std::string> books = {"TRSY1","TRSY2","TRSY3"};

  // 10 trades per product, alternating BUY/SELL, qty cycles 1..5mm, price 99(BUY) / 100(SELL) :contentReference[oaicite:13]{index=13}
  long tid = 1;
  for (const auto& pid : prods) {
    for (int i=0;i<10;++i) {
      bool buy = (i%2==0);
      long qty = (1 + (i%5)) * 1000000L;
      double px = buy ? 99.0 : 100.0;
      std::string book = books[i % books.size()];
      out << "T" << tid++ << "," << pid << "," << FormatPriceFractional(px) << "," << book
          << "," << qty << "," << (buy ? "BUY" : "SELL") << "\n";
    }
  }
}

static void GenInquiries(const std::string& file) {
  std::ofstream out(file);
  auto prods = Products();
  long iid = 1;

  // 10 inquiries per product with state RECEIVED :contentReference[oaicite:14]{index=14}
  for (const auto& pid : prods) {
    for (int i=0;i<10;++i) {
      bool buy = (i%2==0);
      long qty = (1 + (i%5)) * 1000000L;
      // price field included; start at 100-000 (100.0)
      out << "I" << iid++ << "," << pid << "," << (buy ? "BUY" : "SELL") << ","
          << qty << "," << "100-000" << "," << "RECEIVED" << "\n";
    }
  }
}

static void GenMarketData(const std::string& file, long N_per_product) {
  std::ofstream out(file);
  auto prods = Products();

  // We use the line format:
  // productId|bidPx:qty;...|offerPx:qty;...
  // 5 levels deep; top sizes: 10,20,30,40,50mm; spreads widen by 1/128 up to 1/32 then back :contentReference[oaicite:15]{index=15}
  std::vector<long> sizes = {10000000,20000000,30000000,40000000,50000000};

  for (const auto& pid : prods) {
    double mid = 99.0;
    bool up = true;

    // top-of-book spread cycles: 1/128, 1/64, 3/128, 1/32, ... then back
    std::vector<double> spreads = {1.0/128.0, 1.0/64.0, 3.0/128.0, 1.0/32.0, 3.0/128.0, 1.0/64.0};
    std::size_t sp_idx = 0;

    for (long i=0;i<N_per_product;++i) {
      double top_sp = spreads[sp_idx];
      sp_idx = (sp_idx + 1) % spreads.size();

      double bid0 = mid - top_sp/2.0;
      double off0 = mid + top_sp/2.0;

      std::string bid_stack, off_stack;
      for (int lvl=0; lvl<5; ++lvl) {
        // widen by 1/256 each level
        double level_sp = top_sp + lvl * Tick256();
        double bpx = mid - level_sp/2.0;
        double opx = mid + level_sp/2.0;

        if (lvl) { bid_stack += ";"; off_stack += ";"; }
        bid_stack += FormatPriceFractional(bpx) + ":" + std::to_string(sizes[lvl]);
        off_stack += FormatPriceFractional(opx) + ":" + std::to_string(sizes[lvl]);
      }

      out << pid << "|" << bid_stack << "|" << off_stack << "\n";

      mid += up ? Tick256() : -Tick256();
      if (mid >= 101.0) up = false;
      if (mid <= 99.0) up = true;
    }
  }
}

int main(int argc, char** argv) {
  long N = 1000; // demo; set to 1000000 for full assignment
  if (argc > 1) N = std::stol(argv[1]);

  GenPrices("prices.txt", N);
  GenTrades("trades.txt");
  GenInquiries("inquiries.txt");
  GenMarketData("marketdata.txt", N);

  std::cout << "Generated prices.txt, trades.txt, inquiries.txt, marketdata.txt\n";
}
