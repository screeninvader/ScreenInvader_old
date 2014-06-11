#ifndef PRINT_VISITOR_HPP_
#define PRINT_VISITOR_HPP_

#include <string>
#include <iostream>
#include "path.hpp"

namespace janosh {

class PrintVisitor {
public:
  std::ostream& out;

  PrintVisitor(std::ostream& out) :
      out(out) {
  }

  virtual ~PrintVisitor() {
  }

  virtual void beginArray(const Path& p, bool parentIsArray, bool first) {
  }

  virtual void endArray(const Path& p) {
  }

  virtual void beginObject(const Path& p, bool parentIsArray, bool first) {
  }

  virtual void endObject(const Path& p) {
  }

  virtual void begin() {
  }

  virtual void close() {
  }

  virtual void record(const Path& p, const string& value, bool array, bool first) {
  }
};
} /* namespace janosh */

#endif /* PRINT_VISITOR_HPP_ */
