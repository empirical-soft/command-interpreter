#include <iostream>
#include "command_interface.hpp"

// example of a non-member function
int add(int x, int y) {
  return x + y;
}

class Arithmetic : public CommandInterface {
  // example of a static member function
  static int inc(int x) {
    return x + 1;
  }

  // example of a non-static member function
  int twice(int x) {
    return 2 * x;
  }

  // just need to register the commands with a help string
  void register_commands() override {
    register_command(add, "add", "Add two numbers");
    register_command(inc, "inc", "Increment a number");
    register_command(&Arithmetic::twice, "twice", "Double a number");
  }
} arithmetic;

// regression tests
int ret_val = 0;
void test(const std::string& text, const std::string& expected) {
  std::string result = arithmetic.eval(text);
  if (result != expected) {
    std::cout << "Text: " << text << std::endl;
    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Result: " << result << std::endl;
    ret_val = 1;
  }
}

// bring it altogether
int main() {
  test("inc 17", "18");
  test("add 4 5\n", "9");
  test("twice 7", "14");

  test("inc", "Error: expected 1 argument; got 0");
  test("inc 1 7", "Error: expected 1 argument; got 2");
  test("inc 1.7", "Error: invalid argument type at position 0; expected type i");

  test("add 4", "Error: expected 2 arguments; got 1");
  test("add 4 5 6", "Error: expected 2 arguments; got 3");
  test("add 4.4 5", "Error: invalid argument type at position 0; expected type i");
  test("add 4 5.5", "Error: invalid argument type at position 1; expected type i");

  test("help", "add    Add two numbers\ninc    Increment a number\ntwice  Double a number\nhelp   Show this help");

  return ret_val;
}

