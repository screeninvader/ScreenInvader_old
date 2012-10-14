#ifndef _JANOSH_BASH_HPP
#define _JANOSH_BASH_HPP

#include "dbpath.hpp"
#include <string>
#include <stack>
#include <iostream>

using std::string;
using std::cerr;
using std::endl;

namespace janosh {
  class BashPrintVisitor {
    std::ostream& out;

  public:
    BashPrintVisitor(std::ostream& out) :
        out(out){
    }

    void beginArray(const string& key, bool first) {
    }

    void endArray(const string& key) {
    }

    void beginObject(const string& key, bool first) {
    }

    void endObject(const string& key) {
    }

    void begin() {
      out << "( ";
    }

    void close() {
      out << ")" << endl;
    }

    void record(const string& key, const string& value, bool array, bool first) {
        string stripped = value;
        replace(stripped.begin(), stripped.end(), '\n', ' ');
        out  << '[' << key << "]='" << stripped << "' ";
    }
  };
}
#endif
