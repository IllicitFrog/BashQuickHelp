#include "app.hpp"

int main(int argc, const char *argv[]) {
  std::string partial;
  if (argc > 1) {
    partial = argv[1];
  }
  helpme::UI app(partial);
  app.run();
  return 0;
}
