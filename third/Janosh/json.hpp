#ifndef _JANOSH_JSON_HPP
#define _JANOSH_JSON_HPP

#include "print_visitor.hpp"


namespace janosh {
using std::string;
using std::endl;

class JsonPrintVisitor : public PrintVisitor {
    const string escape(const string& s);
  public:
    JsonPrintVisitor(std::ostream& out);
    virtual void beginArray(const Path& p, bool parentIsArray, bool first);
    virtual void endArray(const Path& p);
    virtual void beginObject(const Path& p, bool parentIsArray, bool first);
    virtual void endObject(const Path& p);
    virtual void record(const Path& p, const string& value, bool parentIsArray, bool first);
  };
}

#endif
