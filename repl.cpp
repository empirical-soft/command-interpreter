#include <iostream>
#include "command_interface.hpp"

class Arithmetic : public CommandInterface {
  static int add(int x, int y) {
    return x + y;
  }

  static int inc(int x) {
    return x + 1;
  }

  void register_commands() override {
    register_command(add, "Add two numbers");
    register_command(inc, "Increment a number");
  }
};

int main() {
  Arithmetic arithmetic;
  std::string text;
  std::cout << ">>> ";
  while (std::getline(std::cin, text)) {
    std::cout << arithmetic.eval(text) << std::endl << std::endl;
    std::cout << ">>> ";
  }
  return 0;
}

