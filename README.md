### Command Interpreter

This header-only library makes it easy to add command evaluation to a C++ program.

```cpp
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
```

A command is simply a function. Just register any function with a helper string; type safety occurs automatically.

We can create a simple REPL with the above interpreter.

```cpp
#include <iostream>

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
```

And then we have a command interpreter.

```
>>> help
add   Add two numbers
inc   Increment a number
help  Show this help

>>> add 3 4
7

>>> inc 21
22

>>> add 3 4 5
Error: expected 2 arguments; got 3

>>> add 3 four
Error: invalid argument type at position 1; expected type i

```

Command Interpreter was created for adding a way to query state in microservices, like getting the list of outstanding orders in a trading system.

**This library requires Boost's lexical cast and preprocessor macros.**
