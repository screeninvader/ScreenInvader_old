#ifndef _JANOSH_JSON_HPP
#define _JANOSH_JSON_HPP

#include "dbpath.hpp"
#include <string>
#include <stack>
#include <iostream>

using std::string;
using std::endl;

namespace janosh {
  class JsonPrintVisitor {
    enum container_type {
      Object, Array, None
    };

    struct Frame {
      string name;
      container_type type;

      Frame(string name, container_type type) :
          name(name), type(type) {
      }
    };

    std::ostream& out;
    std::stack<Frame> hierachy;

  public:
    JsonPrintVisitor(std::ostream& out) :
        out(out){
    }

    void beginArray(const string& key, bool first) {
      DBPath p(key);
      string name = p.name();

      if (!first) {
        this->out << ',' << endl;
      }

      if (name.length() == 0)
        this->out << "[ " << endl;
      else
        this->out << '"' << name.erase(0, 1) << "\": [ " << endl;
    }

    void endArray(const string& key) {
      this->out << " ] " << endl;
    }

    void beginObject(const string& key, bool first) {
      DBPath p(key);
      string name = p.name();

      if (!first) {
        this->out << ',' << endl;
      }

      if (name.length() == 0)
        this->out << "{ " << endl;
      else
        this->out << '"' << name.erase(0, 1) << "\": { " << endl;
    }

    void endObject(const string& key) {
      this->out << " } " << endl;
    }

    void record(const string& key, const string& value, bool array, bool first) {
      DBPath p(key);
      string name = p.name();

      if (array) {
        if (!first) {
          this->out << ',' << endl;
        }

        this->out << '\"' << value << '\"';
      } else {
        if (!first) {
          this->out << ',' << endl;
        }

        this->out << '"' << name.erase(0, 1) << "\":\"" << value << "\"";
      }
    }

    void begin() {
      this->out << "{" << endl;
    }

    void close() {
      this->out << "}" << endl;
    }
  };
}

#endif
