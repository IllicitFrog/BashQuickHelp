#pragma once

#include <ftxui/component/component.hpp>
namespace helpme {

// Colors
struct colors {
  static inline ftxui::Color comments = ftxui::Color::White;
  static inline ftxui::Color execute = ftxui::Color::Green;
  static inline ftxui::Color title = ftxui::Color::Cyan;
  static inline ftxui::Color desc = ftxui::Color::Yellow;
  static inline ftxui::Color selected = ftxui::Color::Red;
  static inline ftxui::Color other = ftxui::Color::Blue;
  static inline ftxui::Color exclaim = ftxui::Color::Magenta;
};
} // namespace helpme
