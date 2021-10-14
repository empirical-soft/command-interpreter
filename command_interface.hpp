/*
 * Copyright (C) 2021 Empirical Software Solutions, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

// store the help string and attempt to invoke command
#define register_command(func, name, help) \
  commands_.emplace_back(name, help); \
  if (func_ == name) result_ = call_command(func);

/*
 * Make C++ functions available to an interpreter.
 *
 * Note that the functions cannot be templated or overloaded.
 */

class CommandInterface {
 protected:

  // individual command info
  struct Command {
    std::string name;
    std::string help;
    Command(const std::string& n, const std::string& h) : name(n), help(h) {}
  };

  // list of user commands
  std::vector<Command> commands_;

  // function, arguments, and the result
  std::string func_;
  std::vector<std::string> args_;
  std::string result_;

  // report wrong number of arguments
  std::string err_num_args(size_t expected, size_t got) {
    std::ostringstream oss;
    oss << "Error: expected " << expected << " argument";
    if (expected != 1) {
      oss << 's';
    }
    oss << "; got " << got;
    return oss.str();
  }

  // report wrong argument type
  std::string err_type_args(const std::string& expected, size_t position) {
    std::ostringstream oss;
    oss << "Error: invalid argument type at position " << position
        << "; expected type " << expected;
    return oss.str();
  }

  // split the text into function and arguments
  void parse(const std::string& text) {
    func_.clear();
    args_.clear();

    std::string word;
    for (char c: text) {
      if (std::isspace(c)) {
        if (!word.empty()) {
          args_.push_back(word);
          word.clear();
        }
      }
      else {
        word.push_back(c);
      }
    }
    if (!word.empty()) {
      args_.push_back(word);
    }

    if (!args_.empty()) {
      func_ = args_[0];
      args_.erase(args_.begin());
    }
  }

  // display user's commands
  std::string help() {
    // find longest func name
    size_t longest = 0;
    for (auto& c: commands_) {
      longest = std::max(longest, c.name.size());
    }
    longest += 2;

    // record each func name padded with space, and then the help text
    std::string str;
    for (auto& c: commands_) {
      str += c.name;
      for (size_t i = 0; i < longest - c.name.size(); i++) {
        str += ' ';
      }
      str += c.help + '\n';
    }
    str.pop_back();
    return str;
  }

  // Generate the call_command functions for varying numbers of parameters.
  // The call_command will then template-match the passed function's
  // arguments, which allows lexical_cast to know what type to convert
  // the user's string to. Ie., foo(int) will require the argument string
  // to be cast to a single integer.

#define REP(z, n, text) , text##n

#define REP_COMMA(z, n, text) BOOST_PP_COMMA_IF(n) text##n

#define CAST(z, n, text) \
    T##n x##n; \
    try { \
      x##n = boost::lexical_cast<T##n>(args_[n]); \
    } \
    catch (boost::bad_lexical_cast& err) { \
      return err_type_args(typeid(x##n).name(), n); \
    }

#define CALL_COMMAND(z, n, text) \
  template<class R BOOST_PP_REPEAT(n, REP, class T)> \
  std::string call_command(R (*func)( BOOST_PP_REPEAT(n, REP_COMMA, T) )) { \
    if (args_.size () != n) { \
      return err_num_args(n, args_.size()); \
    } \
    BOOST_PP_REPEAT(n, CAST, ~) \
    return boost::lexical_cast<std::string>(func( BOOST_PP_REPEAT(n, REP_COMMA, x) )); \
  } \
  template<class R, class C BOOST_PP_REPEAT(n, REP, class T)> \
  std::string call_command(R (C::*func)( BOOST_PP_REPEAT(n, REP_COMMA, T) )) { \
    if (args_.size () != n) { \
      return err_num_args(n, args_.size()); \
    } \
    BOOST_PP_REPEAT(n, CAST, ~) \
    C* obj = (C*)(this); \
    return boost::lexical_cast<std::string>((obj->*func)( BOOST_PP_REPEAT(n, REP_COMMA, x) )); \
  }

BOOST_PP_REPEAT(8, CALL_COMMAND, ~)

#undef REP
#undef REP_COMMA
#undef CAST
#undef CALL_COMMAND

 public:

  // evaluate the user's input
  std::string eval(const std::string& text) {
    parse(text);
    result_.clear();
    commands_.clear();

    // the result will be overwritten if the command is recognized
    if (!func_.empty()) {
      result_ = "Unrecognized command: " + func_;
      register_commands();
      register_command(&CommandInterface::help, "help", "Show this help");
    }

    return result_;
  }

  // user must define with each command
  virtual void register_commands() = 0;
};

