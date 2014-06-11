#ifndef _JANOSH_BASH_HPP
#define _JANOSH_BASH_HPP

#include "print_visitor.hpp"

namespace janosh {
using std::string;

class BashPrintVisitor : public PrintVisitor {
public:
  BashPrintVisitor(std::ostream& out);
  virtual void begin();
  virtual void close();
  virtual void record(const Path& p, const string& value, bool array, bool first);
};
}
#endif
