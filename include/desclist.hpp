#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace helpme {
using namespace ftxui;

class desclist {
  float scroll_y = 0.0f;
  int cat = 0;
  int count;
  float inc = 0.0f;

public:
  void setCat(int);
  void scroll_up();
  void scroll_down();
  void scroll_top();
  Component getQuickHelp(ScreenInteractive &);

private:
  std::vector<Element> getDesc(int width);
};
} // namespace helpme
