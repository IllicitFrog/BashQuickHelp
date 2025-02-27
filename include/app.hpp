#pragma once

#include "desclist.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace helpme {
class UI {
  ftxui::ScreenInteractive screen;
  desclist descList;

  std::vector<Element> qCommands;
  std::vector<Element> qCategories;

  // Command Help
  std::string commandPartial;
  std::string last = "start";
  int lastCategory = 255;
  int selectedCategory = 0;

  bool valid = false;

public:
  UI(std::string);

  void run();

private:
  ftxui::Component getSystem();
  ftxui::Component getCategories();
  ftxui::Component getCommands();
  int setCategory();
};
} // namespace helpme
