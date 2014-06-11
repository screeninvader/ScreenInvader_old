#ifndef _JANOSH_HPP
#define _JANOSH_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "trigger_base.hpp"
#include "settings.hpp"
#include "record.hpp"
#include "format.hpp"
#include "print_visitor.hpp"

namespace janosh {

namespace js = json_spirit;
namespace fs = boost::filesystem;
using std::string;
using std::vector;
using std::map;
using std::vector;
using std::istream;
using std::ostream;

class Command;
typedef map<const std::string, Command*> CommandMap;

class Janosh {
public:
  Settings settings_;
  TriggerBase triggers_;
  CommandMap cm_;

  Janosh();
  ~Janosh();

  void setFormat(Format f);
  void open(bool readOnly);
  bool isOpen();
  void close();
  size_t process(int argc, char** argv);

  size_t loadJson(const string& jsonfile);
  size_t loadJson(istream& is);

  size_t makeArray(Record target, size_t size = 0, bool boundsCheck = true);
  size_t makeObject(Record target, size_t size = 0);
  size_t makeDirectory(Record target, Value::Type type, size_t size = 0);
  size_t get(vector<Record> targets, std::ostream& out);
  size_t size(Record target);
  size_t remove(Record& target, bool pack = true);

  size_t add(Record target, const string& value);
  size_t replace(Record target, const string& value);
  size_t set(Record target, const string& value);
  size_t append(Record target, const string& value);
  size_t append(vector<string>::const_iterator begin, vector<string>::const_iterator end, Record dest);

  size_t move(Record& src, Record& dest);
  size_t replace(Record& src, Record& dest);
  size_t append(Record& src, Record& dest);
  size_t copy(Record& src, Record& dest);
  size_t shift(Record& src, Record& dest);

  size_t dump(ostream& out);
  size_t hash();
  size_t truncate();
  static Janosh* getInstance();
private:
  static Janosh* instance_;
  Format format;
  bool open_;
  string filename;
  js::Value rootValue;

  Format getFormat();
  void setContainerSize(Record rec, const size_t s);
  void changeContainerSize(Record rec, const size_t by);
  size_t load(const Path& path, const string& value);
  size_t load(js::Value& v, Path& path);
  size_t load(js::Object& obj, Path& path);
  size_t load(js::Array& array, Path& path);
  bool boundsCheck(Record p);
  Record makeTemp(const Value::Type& t);

  size_t recurse(Record& travRoot, PrintVisitor* vis, ostream& out);
};

}
#endif
