#ifndef CSV_UTILS_HPP
#define CSV_UTILS_HPP

#include <sstream>
#include <string>
#include <vector>

inline std::vector<std::string> Split(const std::string& s, char delim) {
  std::vector<std::string> out;
  std::string item;
  std::istringstream iss(s);
  while (std::getline(iss, item, delim)) out.push_back(item);
  return out;
}

#endif
