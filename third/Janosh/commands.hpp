#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include <string>
#include <iostream>
#include <map>
#include "janosh.hpp"

namespace janosh {
using std::pair;
using std::string;
using std::vector;

typedef map<const string, Command*> CommandMap;

class Command {
protected:
  Janosh * janosh;
public:
  typedef pair<int32_t, string> Result;

  Command(Janosh* janosh) :
      janosh(janosh) {
  }

  virtual ~Command() {
  }

  virtual Result operator()(const vector<string>& params, ostream& out) {
    return {-1, "Not implemented"};
  }
  ;
};

CommandMap makeCommandMap(Janosh* janosh);
}
#endif
