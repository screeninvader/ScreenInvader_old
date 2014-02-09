#ifndef _JANOSH_HPP
#define _JANOSH_HPP

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <algorithm>
#include <exception>
#include <initializer_list>
#include <unistd.h>

#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>
#include <boost/function.hpp>
#include <boost/interprocess/creation_tags.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <kcpolydb.h>
#include "trigger_base.hpp"
#include "settings.hpp"
#include "logger.hpp"
#include "record.hpp"
#include "json_spirit/json_spirit.h"
#include "json.hpp"
#include "bash.hpp"
#include "format.hpp"

  namespace kc = kyotocabinet;
  namespace js = json_spirit;
  namespace fs = boost::filesystem;
  using std::cerr;
  using std::cout;
  using std::endl;
  using std::string;
  using std::vector;
  using std::map;
  using std::istringstream;
  using std::ifstream;
  using std::exception;
namespace janosh {
  class Command;
  typedef map<const std::string, Command*> CommandMap;

  class Janosh {
public:
    typedef boost::function<void(int)> ExitHandler;
    Settings settings_;
    TriggerBase triggers_;
    CommandMap cm_;

      Janosh();
    ~Janosh();

    void setFormat(Format f) ;
    void open(bool readOnly);
    void close();
    size_t process(int argc, char** argv);

    size_t loadJson(const string& jsonfile);
    size_t loadJson(std::istream& is);

    size_t makeArray(Record target, size_t size = 0, bool boundsCheck=true);
    size_t makeObject(Record target, size_t size = 0);
    size_t makeDirectory(Record target, Value::Type type, size_t size = 0);
    size_t get(Record target, std::ostream& out);
    size_t size(Record target);
    size_t remove(Record& target, bool pack=true);

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

    size_t dump(std::ostream& out);
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
    bool isOpen();
    void terminate(int code);
    void setContainerSize(Record rec, const size_t s);
    void changeContainerSize(Record rec, const size_t by);
    size_t load(const Path& path, const string& value);
    size_t load(js::Value& v, Path& path);
    size_t load(js::Object& obj, Path& path);
    size_t load(js::Array& array, Path& path);
    bool boundsCheck(Record p);
    Record makeTemp(const Value::Type& t);

    template<typename Tvisitor>
     size_t recurse(Record& travRoot, Tvisitor vis)  {
       size_t cnt = 0;
       std::stack<std::pair<const Component, const Value::Type> > hierachy;
       Record root("/.");

       Record rec(travRoot);
       vis.begin();

       Path last;
       do {
         rec.fetch();
         const Path& path = rec.path();
         const Value& value = rec.value();
         const Value::Type& t = rec.getType();
         const Path& parent = path.parent();

         const Component& name = path.name();
         const Component& parentName = parent.name();

         if (!hierachy.empty()) {
           if (!travRoot.isAncestorOf(path)) {
             break;
           }

           if(!last.above(path) && (
               (!last.isDirectory() && parentName != last.parentName()) ||
               (last.isDirectory() && parentName != last.name()))){
             while(!hierachy.empty() && hierachy.top().first != parentName) {
               if (hierachy.top().second == Value::Array) {
                 vis.endArray(path);
               } else if (hierachy.top().second == Value::Object) {
                 vis.endObject(path);
               }
               hierachy.pop();
             }
           }
         }

         if (t == Value::Array) {
           Value::Type parentType;
           if(hierachy.empty())
             parentType = Value::Array;
           else
             parentType = hierachy.top().second;

           hierachy.push({name, Value::Array});
           vis.beginArray(path, parentType == Value::Array, last.isEmpty() || last == parent);
         } else if (t == Value::Object) {
           Value::Type parentType;
           if(hierachy.empty())
             parentType = Value::Array;
           else
             parentType = hierachy.top().second;

           hierachy.push({name, Value::Object});
           vis.beginObject(path, parentType == Value::Array, last.isEmpty() || last == parent);
         } else {
           bool first = last.isEmpty() || last == parent;
           if(!hierachy.empty()){
             vis.record(path, value, hierachy.top().second == Value::Array, first);
           } else {
             vis.record(path, value, false, first);
           }
         }
         last = path;
         ++cnt;
       } while (rec.step());

       while (!hierachy.empty()) {
           if (hierachy.top().second == Value::Array) {
             vis.endArray("");
           } else if (hierachy.top().second == Value::Object) {
             vis.endObject("");
           }
           hierachy.pop();
       }

       vis.close();
       return cnt;
     }
  };

  class RawPrintVisitor {
    std::function<void(const string&)> f;
    std::ostream& out;

  public:
    RawPrintVisitor(std::ostream& out) :
        out(out) {
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
    }

    void close() {
    }

    void record(const Path& p, const string& value, bool array, bool first) {
      string stripped = value;
      replace(stripped.begin(), stripped.end(), '\n', ' ');
      out << stripped << endl;
    }
  };
}
#endif
