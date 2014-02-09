#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include "janosh.hpp"
#include <boost/range.hpp>
#include <boost/tokenizer.hpp>
#include <string>
#include <iostream>

namespace janosh {
using std::string;
using std::vector;
using boost::tokenizer;
using boost::char_separator;

typedef map<const std::string, Command*> CommandMap;

class Command {
protected:
  Janosh * janosh;
public:
  typedef std::pair<int32_t, string> Result;

  Command(Janosh* janosh) :
      janosh(janosh) {
  }

  virtual ~Command() {
  }

  virtual Result operator()(const std::vector<string>& params, std::ostream& out) {
    return {-1, "Not implemented"};
  }
  ;
};

CommandMap makeCommandMap(Janosh* janosh);
}
#endif
