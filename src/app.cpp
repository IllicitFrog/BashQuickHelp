#include "app.hpp"
#include "commandlist.hpp"
#include "quickhelp.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/terminal.hpp>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace helpme {
using namespace ftxui;

UI::UI(std::string partial)
    : screen(ftxui::ScreenInteractive::Fullscreen()), commandPartial(partial) {}

void UI::run() {
  // Create UI
  int scroll_y = 0;
  int swidth = 0;
  int sheight = 0;
  bool valid = false;
  std::vector<Element> systeminfo = getsystem();

  Element sysinfo =
      window(color(title, text("System Info")) | bold, vflow(systeminfo)) |
      size(HEIGHT, EQUAL, 7);

  if (!commandList.empty()) {
    selectedCategory = setCategory();
  }

  auto mainWindow =
      Renderer({[&] {
        swidth = screen.dimx();
        sheight = screen.dimy();

        renderDesc(scroll_y, sheight - 3, swidth - 43);
        renderCommands();
        renderCategories(swidth);

        Element categoryList =
            window(color(title, text("Quick Help - LEFT/RIGHT")) | bold,
                   hflow(qCategories) | center);
        Element descList =
            window(color(title, text("Description - PAGEUP/PAGEDOWN")) | bold,
                   vflow(qDesc));
        Element cmdList = window(color(title, text("Commands")) | bold,
                                 vbox({window(color(desc, text("Search:")),
                                              text(commandPartial) | center),
                                       vflow(qCommands)}));

        if (!valid) {
          screen.PostEvent(Event::Custom);
          valid = true;
          lastCategory = 255;
        }

        return vbox(
            {color(desc, text("Bash Quick Help - (ESC to exit)")) | center |
                 bold,
             categoryList | xframe | xflex | size(HEIGHT, EQUAL, 3),
             hbox({vbox({sysinfo, cmdList | size(WIDTH, EQUAL, 40) |
                                      size(HEIGHT, EQUAL, cmdIndex + 5)}),
                   descList | frame | flex}) |
                 xframe | xflex | size(HEIGHT, EQUAL, sheight - 4)});
      }}) |
      CatchEvent([&](Event event) {
        if (event == Event::ArrowRight) {
          scroll_y = 0;
          lastScroll = 0;
          if (selectedCategory == categories.size() - 1)
            selectedCategory = 0;
          else
            selectedCategory++;
        } else if (event == Event::ArrowLeft) {
          scroll_y = 0;
          lastScroll = 0;
          if (selectedCategory == 0)
            selectedCategory = categories.size() - 1;
          else
            selectedCategory--;
        } else if (event == Event::PageDown && descIndex >= screen.dimy() - 7) {
          scroll_y += 1;
        } else if (event == Event::PageUp) {
          if (scroll_y > 0)
            scroll_y -= 1;
        } else if (event == Event::Escape) {
          screen.Exit();
          return false;
        } else if (event == Event::Backspace) {
          commandPartial = commandPartial.substr(0, commandPartial.size() - 1);
        } else {
          std::string e = event.character();
          if (e[0] >= 'a' && e[0] <= 'z' || e[0] >= 'A' && e[0] <= 'Z')
            commandPartial += event.character();
        }
        return true;
      });
  screen.Loop(mainWindow);
}

void UI::renderDesc(int scroll_y, int height, int width) {
  if (selectedCategory != lastCategory || scroll_y != lastScroll) {
    qDesc.clear();
    for (int i = scroll_y; qDesc.size() < height - 4 &&
                           i < categories[selectedCategory].examples.size();
         i++) {
      char formatter = categories[selectedCategory].examples[i][0];

      switch (formatter) {
      case '#': {
        if (categories[selectedCategory].examples[i].size() > width - 2) {
          std::string &temp = categories[selectedCategory].examples[i];
          int j = 0;
          while (categories[selectedCategory].examples[i].size() - j >
                 width - 2) {
            for (int h = j + width - 2; h > 0; h--) {
              if (temp[h + j] == ' ') {
                qDesc.push_back(color(comments, text(temp.substr(1, h))));
                j += h;
                break;
              }
            }
          }
          qDesc.push_back(color(comments, text(temp.substr(j, temp.size()))));

        } else {
          qDesc.push_back(
              color(comments, text(categories[selectedCategory].examples[i])));
        }
        break;
      }
      case '@':
        qDesc.push_back(text(""));
        qDesc.push_back(color(
            desc, text(categories[selectedCategory].examples[i].substr(1))));
        break;
      case ' ':
        qDesc.push_back(color(
            execute, text(categories[selectedCategory].examples[i].substr(1))));
        break;
      case '$':
        qDesc.push_back(text(""));
        qDesc.push_back(
            color(execute, text(categories[selectedCategory].examples[i])));
        break;
      default:
        qDesc.push_back(
            color(other, text(categories[selectedCategory].examples[i])));
        break;
      }
    }

    descIndex = qDesc.size();
    if (qDesc.size() > height - 5) {
      qDesc.pop_back();
      qDesc.push_back(color(selected, text("-----SCROLL-----")));
    }
    lastScroll = scroll_y;
  }
}

void UI::renderCategories(int width) {
  if (selectedCategory != lastCategory) {
    qCategories.clear();
    int cols = (width - 4) / 23;
    int mod = std::min(std::max(selectedCategory - cols / 2, 0),
                       int(categories.size() - cols));
    for (int i = mod; i < cols + mod; i++) {
      if (selectedCategory == i) {
        qCategories.push_back(
            dbox({color(selected, text(categories[i].name) | bold | center)}) |
            size(WIDTH, EQUAL, 23));
      } else {
        qCategories.push_back(dbox({text(categories[i].name) | center}) |
                              size(WIDTH, EQUAL, 23));
      }
    }
    lastCategory = selectedCategory;
  }
}

void UI::renderCommands() {
  if (commandPartial != last) {
    int command = 0;
    qCommands.clear();
    for (int i = 0; i < commandList.size(); i++) {
      if (commandList[i].name.find(commandPartial) != std::string::npos) {
        command = i;
        break;
      }
    }

    qCommands.push_back(color(selected, text(commandList[command].name)) |
                        bold);
    qCommands.push_back(color(desc, text(commandList[command].description)));
    for (auto &cl : commandList[command].examples) {
      if (cl.substr(0, 1) == "#") {
        qCommands.push_back(color(comments, text(cl)));
      } else {
        qCommands.push_back(color(execute, text(cl)));
      }
    }
    cmdIndex = qCommands.size();
    last = commandPartial;
  }
}

std::vector<Element> UI::getsystem() {
  struct utsname name;
  std::vector<Element> systeminfo;

  std::chrono::milliseconds uptime(0u);
  struct sysinfo x;
  if (sysinfo(&x) == 0) {
    uptime = std::chrono::milliseconds(
        static_cast<unsigned long long>(x.uptime) * 1000ULL);
  }

  int days = uptime.count() / 86400000;
  int hours = (uptime.count() / 3600000) % 24;
  int minutes = (uptime.count() / 60000) % 60;
  systeminfo.push_back(text("Uptime: " + std::to_string(days) + " days " +
                            std::to_string(hours) + " hours " +
                            std::to_string(minutes) + " minutes "));

  static char ip[32];
  static char gateway[32];
  FILE *f = popen("ip r | grep default | awk '{print $3}'", "r");
  int c, i = 0;
  while ((c = getc(f)) != EOF)
    i += sprintf(gateway + i, "%c", c);
  pclose(f);

  f = popen("ip r | grep default | awk '{print $9}'", "r");
  i = 0;
  c = 0;
  while ((c = getc(f)) != EOF)
    i += sprintf(ip + i, "%c", c);
  pclose(f);

  if (uname(&name) == 0) {
    systeminfo.push_back(text(std::string(name.sysname) + "-" +
                              std::string(name.machine) + " " +
                              std::string(name.release)));
    systeminfo.push_back(text("Hostname: " + std::string(name.nodename)));
  }

  if (ip[0] != '\0') {
    systeminfo.push_back(text("IP Address: " + std::string(ip)));
  }
  if (gateway[0] != '\0') {
    systeminfo.push_back(text("Gateway: " + std::string(gateway)));
  }

  return systeminfo;
}

int UI::setCategory() {
  for (int i = 0; i < categories.size(); i++) {
    for (auto &eg : categories[i].examples) {
      if (eg.find("$" + commandPartial) != std::string::npos) {
        return i;
      }
    }
  }
  return 0;
}

} // namespace helpme
