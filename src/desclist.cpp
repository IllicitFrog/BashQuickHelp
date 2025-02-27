#include "desclist.hpp"
#include "quickhelp.hpp"
#include "colors.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace helpme {

Component desclist::getQuickHelp(ScreenInteractive &screen) {

  return Renderer([this, &screen]() {
    std::vector<Element> desc = getDesc(screen.dimx() - 46);
    count = desc.size();

    Element cc;
    if (count < screen.dimy() - 4) {
      cc = vbox(desc) | focusPositionRelative(0, scroll_y) | frame | flex;
    } else {
      inc = float(screen.dimy() - 4) / float(count);
      SliderOption<float> option_y;
      option_y.value = &scroll_y;
      option_y.min = 0.f;
      option_y.max = 1.f;
      option_y.increment = 0.1f;
      option_y.direction = Direction::Down;
      option_y.color_active = Color::Cyan;

      cc = hbox({vbox(desc) | focusPositionRelative(0, scroll_y) | frame | flex,
                 Slider(option_y)->Render()});
    }

    return window(color(colors::title, text("Quick Help - Page Up/Down")), cc) |
           size(HEIGHT, EQUAL, screen.dimy() - 4) | xflex;
  });
}

void desclist::setCat(int cat) { this->cat = cat; }

void desclist::scroll_up() { scroll_y = std::max(0.0f, scroll_y - inc); }

void desclist::scroll_down() { scroll_y = std::min(1.0f, scroll_y + inc); }

void desclist::scroll_top() { scroll_y = 0.1; }

std::vector<Element> desclist::getDesc(int width) {
  std::vector<Element> qDesc;
  for (auto &eg : categories[cat].examples) {
    char formatter = eg[0];
    switch (formatter) {
    case '#': {
      if (eg.size() > width) {
        std::string &temp = eg;
        int j = 0;
        while (eg.size() - j > width) {
          for (int h = j + width; h > 0; h--) {
            if (temp[h + j] == ' ') {
              qDesc.push_back(color(colors::comments, text(temp.substr(0, h))));
              j += h;
              break;
            }
          }
        }
        qDesc.push_back(
            color(colors::comments, text(temp.substr(j, temp.size()))));

      } else {
        qDesc.push_back(color(colors::comments, text(eg)));
      }
      break;
    }
    case '@':
      qDesc.push_back(text(""));
      qDesc.push_back(color(colors::desc, text(eg.substr(1))));
      break;
    case ' ':
      qDesc.push_back(color(colors::execute, text(eg.substr(1))));
      break;
    case '$':
      qDesc.push_back(text(""));
      qDesc.push_back(color(colors::execute, text(eg)));
      break;
    case '!':
      qDesc.push_back(text(""));
      qDesc.push_back(color(colors::exclaim, text(eg)));
      break;
    default:
      qDesc.push_back(text(""));
      qDesc.push_back(color(colors::other, text(eg)));
      break;
    }
  }
  return qDesc;
}
} // namespace helpme
