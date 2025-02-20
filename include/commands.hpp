#pragma once

#include <string>
#include <vector>

namespace helpme {
struct Commands {
  std::string name;
  std::string description;
  std::vector<std::string> examples;
};
} // namespace helpme
