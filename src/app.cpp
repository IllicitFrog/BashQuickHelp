#include "app.hpp"
#include "commandlist.hpp"
#include "quickhelp.hpp"
#include "colors.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace helpme {
using namespace ftxui;

UI::UI(std::string partial)
    : screen(ftxui::ScreenInteractive::Fullscreen()), commandPartial(partial) {
  if (!commandList.empty()) {
    selectedCategory = setCategory();
    descList.setCat(selectedCategory);
  }
}

void UI::run() {
  Component title = Renderer([] {
    return text("Quick Help (ESC to Quit)") | center | bold |
           color(colors::title) | size(HEIGHT, EQUAL, 1) | xflex;
  });

  Component mainWindow =
      Container::Vertical(
          {title, getCategories(),
           Container::Horizontal({Container::Vertical({
                                      getSystem(),
                                      getCommands(),
                                  }),
                                  descList.getQuickHelp(screen)})}) |
      flex;

  mainWindow |= CatchEvent([&](Event event) {
    if (event == Event::ArrowRight) {
      if (selectedCategory == categories.size() - 1)
        selectedCategory = 0;
      else
        selectedCategory++;
      descList.scroll_top();
      descList.setCat(selectedCategory);
    } else if (event == Event::ArrowLeft) {
      if (selectedCategory == 0)
        selectedCategory = categories.size() - 1;
      else
        selectedCategory--;
      descList.scroll_top();
      descList.setCat(selectedCategory);
    } else if (event == Event::PageDown) {
      descList.scroll_down();
    } else if (event == Event::PageUp) {
      descList.scroll_up();
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

Component UI::getCategories() {
  return Renderer({[&] {
    if (selectedCategory != lastCategory) {
      qCategories.clear();
      int cols = (screen.dimx() - 4) / 23;
      int mod = std::min(std::max(selectedCategory - cols / 2, 0),
                         int(categories.size() - cols));
      for (int i = mod; i < cols + mod; i++) {
        if (selectedCategory == i) {
          qCategories.push_back(
              dbox({color(colors::selected,
                          text(categories[i].name) | bold | center)}) |
              size(WIDTH, EQUAL, 23));
        } else {
          qCategories.push_back(dbox({text(categories[i].name) | center}) |
                                size(WIDTH, EQUAL, 23));
        }
      }
      lastCategory = selectedCategory;
    }
    return window(color(colors::title, text("Categories - Left/Right")),
                  hbox(qCategories) | xflex | center | size(HEIGHT, EQUAL, 1));
  }});
}

Component UI::getCommands() {
  return Renderer({[&] {
    if (commandPartial != last) {
      qCommands.clear();
      int command = 0;
      for (int i = 0; i < commandList.size(); i++) {
        if (commandList[i].name.find(commandPartial) != std::string::npos) {
          command = i;
          break;
        }
      }

      qCommands.push_back(
          color(colors::selected, text(commandList[command].name)) | bold);
      qCommands.push_back(
          color(colors::desc, text(commandList[command].description)));
      for (auto &cl : commandList[command].examples) {
        if (cl.substr(0, 1) == "#") {
          qCommands.push_back(color(colors::comments, text(cl)));
        } else {
          qCommands.push_back(color(colors::execute, text(cl)));
        }
      }
      last = commandPartial;
    }
    if (!valid) {
      screen.PostEvent(Event::Custom);
      valid = true;
      lastCategory = 255;
    }
    return window(color(colors::title, text("Commands")),
                  vbox({window(color(colors::desc, text("Search")),
                               color(colors::execute, text(commandPartial))) |
                            size(HEIGHT, EQUAL, 3) | xflex,
                        vbox(qCommands)}) |
                      size(WIDTH, EQUAL, 40) |
                      size(HEIGHT, EQUAL, 3 + qCommands.size()));
  }});
}

Component UI::getSystem() {
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

  return Renderer([systeminfo] {
    return window(color(colors::title, text("System")), vbox(systeminfo)) |
           size(WIDTH, EQUAL, 40);
  });
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
