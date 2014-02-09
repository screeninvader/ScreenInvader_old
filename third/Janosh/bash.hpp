#ifndef _JANOSH_BASH_HPP
#define _JANOSH_BASH_HPP

#include "record.hpp"
#include <string>
#include <stack>
#include <iostream>
#include <boost/algorithm/string.hpp>

using std::string;

namespace janosh {
  class BashPrintVisitor {
    std::ostream& out;

  public:
    BashPrintVisitor(std::ostream& out) :
        out(out){
    }

    void beginArray(const Path& p, bool parentIsArray, bool first) {
    }

    void endArray(const Path& p) {
    }

    void beginObject(const Path& p, bool parentIsArray, bool first) {
    }

    void endObject(const Path& p) {
    }

    void begin() {
      out << "( ";
    }

    void close() {
      out << ")" << endl;
    }

    void record(const Path& p, const string& value, bool array, bool first) {
        string stripped = value;
        replace(stripped.begin(), stripped.end(), '\n', ' ');
        boost::algorithm::replace_all(stripped, "&",  "&#39;");
        out  << '[' << p.pretty() << "]='" << stripped << "' ";
    }
  };
}
#endif
