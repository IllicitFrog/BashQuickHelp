#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace helpme {
class UI {
  ftxui::ScreenInteractive screen;

  // Command Help
  std::string commandPartial;

  // Quick Help
  int descIndex = 0;
  int cmdIndex = 0;

  // Colors
  ftxui::Color comments = ftxui::Color::White;
  ftxui::Color execute = ftxui::Color::Green;
  ftxui::Color title = ftxui::Color::Cyan;
  ftxui::Color desc = ftxui::Color::Yellow;
  ftxui::Color selected = ftxui::Color::Red;
  ftxui::Color other = ftxui::Color::Blue;

public:
  UI(std::string);

  void run();

private:
  std::vector<ftxui::Element> getsystem();
  std::vector<ftxui::Element> renderDesc(int, int, int, int);
  std::vector<ftxui::Element> renderCategories(int, int);
  std::vector<ftxui::Element> renderCommands();

};
} // namespace helpme
