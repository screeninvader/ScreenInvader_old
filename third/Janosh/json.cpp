#include <iostream>
#include "json.hpp"

namespace janosh {

JsonPrintVisitor::JsonPrintVisitor(std::ostream& out) : PrintVisitor(out) {
}

const string JsonPrintVisitor::escape(const string& s) {
  size_t index = 0;
  string ns = s;
  while (true) {
    index = ns.find("\"", index);
    if (index == string::npos)
      break;
    ns.replace(index, 1, "\\\"");
    index += 2;
  }
  return ns;
}

void JsonPrintVisitor::beginArray(const Path& p, bool parentIsArray, bool first) {
  string name = p.name().pretty();
  if (!first) {
    this->out << ',' << endl;
  }

  if (parentIsArray)
    this->out << "[ " << endl;
  else
    this->out << '"' << name << "\": [ " << endl;
}

void JsonPrintVisitor::endArray(const Path& p) {
  this->out << " ] " << endl;
}

void JsonPrintVisitor::beginObject(const Path& p, bool parentIsArray, bool first) {
  string name = p.name().pretty();
  if (!first) {
    this->out << ',' << endl;
  }

  if (parentIsArray)
    this->out << "{ " << endl;
  else
    this->out << '"' << name << "\": { " << endl;
}

void JsonPrintVisitor::endObject(const Path& p) {
  this->out << " } " << endl;
}

void JsonPrintVisitor::record(const Path& p, const string& value, bool parentIsArray, bool first) {
  string name = p.name().pretty();
  string jsonValue = escape(value);

  if (parentIsArray) {
    if (!first) {
      this->out << ',' << endl;
    }

    this->out << '\"' << jsonValue << '\"';
  } else {
    if (!first) {
      this->out << ',' << endl;
    }

    this->out << '"' << name << "\":\"" << jsonValue << "\"";
  }
}
}
