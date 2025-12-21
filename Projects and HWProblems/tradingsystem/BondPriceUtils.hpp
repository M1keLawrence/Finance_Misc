#ifndef BOND_PRICE_UTILS_HPP
#define BOND_PRICE_UTILS_HPP

#include <cctype>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

inline double ParsePriceMaybeFractional(const std::string& s) {
  // Accept decimal or "100-25+" style.
  // If no '-', parse as double.
  if (s.find('-') == std::string::npos) return std::stod(s);

  // Format: WHOLE-XYz where XY in [00..31], z in [0..7] or '+'
  // '+' means z=4.
  auto dash = s.find('-');
  if (dash == std::string::npos || dash + 1 >= s.size()) throw std::runtime_error("Bad price: " + s);

  int whole = std::stoi(s.substr(0, dash));
  std::string frac = s.substr(dash + 1);
  if (frac.size() != 3) throw std::runtime_error("Bad fractional price: " + s);

  int xy = std::stoi(frac.substr(0, 2));
  char zc = frac[2];
  int z = 0;
  if (zc == '+') z = 4;
  else if (std::isdigit(static_cast<unsigned char>(zc))) z = zc - '0';
  else throw std::runtime_error("Bad fractional z: " + s);

  if (xy < 0 || xy > 31 || z < 0 || z > 7) throw std::runtime_error("Fraction out of range: " + s);

  // xy/32 + z/256
  return static_cast<double>(whole) + static_cast<double>(xy) / 32.0 + static_cast<double>(z) / 256.0;
}

inline std::string FormatPriceFractional(double px) {
  // Convert to nearest 1/256.
  double whole_d = std::floor(px);
  int whole = static_cast<int>(whole_d);
  double frac = px - whole_d;
  int ticks256 = static_cast<int>(std::llround(frac * 256.0));
  if (ticks256 == 256) { whole += 1; ticks256 = 0; }

  int xy = ticks256 / 8;     // /32 in 256ths => 8 ticks
  int z = ticks256 % 8;

  std::ostringstream oss;
  oss << whole << "-";
  if (xy < 10) oss << "0";
  oss << xy;
  oss << (z == 4 ? '+' : static_cast<char>('0' + z));
  return oss.str();
}

#endif
