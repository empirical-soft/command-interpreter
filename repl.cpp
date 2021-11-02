#include <iostream>
#include "command_interpreter.hpp"

class Arithmetic : public CommandInterpreter {
  static int add(int x, int y) {
    return x + y;
  }

  int inc(int x) {
    return x + 1;
  }

  void register_commands() override {
    register_command(add, "add", "Add two numbers");
    register_command(&Arithmetic::inc, "inc", "Increment a number");
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

