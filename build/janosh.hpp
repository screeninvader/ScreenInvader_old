#ifndef _JANOSH_HPP
#define _JANOSH_HPP

#include "json_spirit.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <kcpolydb.h>

#include "dbpath.hpp"
#include "json.hpp"
#include "bash.hpp"

namespace janosh {
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

  enum Format {
    Bash,
    Json,
    Raw
  };

  class TriggerBase {
    typedef std::map<string,string> TargetMap;
    typedef typename TargetMap::value_type Target;

    vector<fs::path> targetDirs;
    map<DBPath, std::set<string> > triggers;
    TargetMap targets;
public:
    TriggerBase(const fs::path& config, const vector<fs::path>& targetDirs);

    int executeTarget(const string& name);

    void executeTrigger(const DBPath p);
    bool findAbsoluteCommand(const string& cmd, string& abs);
    void load(const fs::path& config);
    void load(std::ifstream& is);
    template<typename T> void error(const string& msg, T t, int exitcode=1) {
      LOG_ERR_MSG(msg, t);
      exit(exitcode);
    }
  };

  class Settings {
  public:
    fs::path janoshFile;
    fs::path databaseFile;
    fs::path triggerFile;
    fs::path logFile;
    vector<fs::path> triggerDirs;

    Settings();
    template<typename T> void error(const string& msg, T t, int exitcode=1) {
      LOG_ERR_MSG(msg, t);
      exit(exitcode);
    }
  private:
    bool find(const js::Object& obj, const string& name, js::Value& value);
  };

  class RawPrintVisitor {
    std::ostream& out;

  public:
    RawPrintVisitor(std::ostream& out) :
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
    }

    void close() {
    }

    void record(const string& key, const string& value, bool array, bool first) {
        string stripped = value;
        replace(stripped.begin(), stripped.end(), '\n', ' ');
        out << stripped << endl;
    }
  };

  class Janosh {
  public:
    Settings settings;
    TriggerBase triggers;
    Format format;
    Janosh();

    void setFormat(Format f) ;
    Format getFormat();

    template<typename T> void error(const string& msg, T t, int exitcode=1) {
      LOG_ERR_MSG(msg, t);
      close();
      exit(exitcode);
    }

    void open();
    void close();
    size_t loadJson(const string& jsonfile);
    size_t loadJson(std::istream& is);
    size_t makeArray(DBPath target, size_t size = 0, bool boundsCheck=true);
    size_t makeObject(DBPath target, size_t size = 0);
    size_t makeDirectory(DBPath target, EntryType type, size_t size = 0);
    size_t print(DBPath path, std::ostream& out);
    size_t print(Cursor cur, std::ostream& out);

    size_t add(DBPath path, const string& value);
    size_t replace(DBPath path, const string& value);
    size_t set(Cursor cur, const string& value);
    size_t set(DBPath path, const string& value);
    size_t remove(DBPath cur);
    size_t remove(Cursor cur, size_t n);
    size_t removeChildren(Cursor cur);
    size_t dump();
    size_t hash();
    size_t truncate();
    size_t size(DBPath path);
    size_t size(Cursor cur);
    size_t append(const string& value, Cursor dest);
    size_t append(vector<string>::const_iterator begin, vector<string>::const_iterator end, Cursor dest);
    size_t append(Cursor srcCur, Cursor destCur, size_t n);
    size_t appendChildren(Cursor srcCur, Cursor destCur);
    size_t copy(Cursor srcCursor, Cursor destCursor);
    size_t shift(Cursor srcCur, Cursor destCur);
  private:
    string filename;
    js::Value rootValue;

    void setContainerSize(Cursor cur, const size_t s);
    void changeContainerSize(Cursor cur, const size_t by);

    size_t load(DBPath path, const string& value);
    size_t load(js::Value& v, DBPath& path);
    size_t load(js::Object& obj, DBPath& path);
    size_t load(js::Array& array, DBPath& path);
    bool boundsCheck(DBPath p);
    DBPath makeTemp(EntryType t);

    template<typename Tvisitor>
     size_t recurse(Cursor cur, Tvisitor vis)  {
       size_t cnt = 0;
       std::stack<std::pair<const string, const EntryType> > hierachy;
       DBPath travRoot(cur);
       DBPath root("/.");
       if(travRoot == DBPath("/."))
         cur.step();

       vis.begin();

       DBPath last;

       do {
         const DBPath p(cur);
         const EntryType& t = p.getType();
         const string& name = p.name();
         const DBPath& parent = p.parent();
         const string& parentName = parent.name();

         if (!hierachy.empty()) {
           if (!travRoot.above(p)) {
             break;
           }

           if(!last.above(p) && (
               (!last.isContainer() && parentName != last.parentName()) ||
               (last.isContainer() && parentName != last.name()))){
             while(!hierachy.empty() && hierachy.top().first != parentName) {
               if (hierachy.top().second == EntryType::Array) {
                 vis.endArray(p.key());
               } else if (hierachy.top().second == EntryType::Object) {
                 vis.endObject(p.key());
               }
               hierachy.pop();
             }
           }
         }

         if (t == EntryType::Array) {
           hierachy.push({name, EntryType::Array});
           vis.beginArray(p.key(), last.empty() || last == parent);
         } else if (t == EntryType::Object) {
           hierachy.push({name, EntryType::Object});
           vis.beginObject(p.key(), last.empty() || last == parent);
         } else {
           bool first = last.empty() || last == parent;
           if(!hierachy.empty()){
             vis.record(p.key(), p.val(), hierachy.top().second == EntryType::Array, first);
           } else {
             vis.record(p.key(), p.val(), false, first);
           }
         }
         last = p;
         ++cnt;
       } while (cur.step());

       while (!hierachy.empty()) {
           if (hierachy.top().second == EntryType::Array) {
             vis.endArray("");
           } else if (hierachy.top().second == EntryType::Object) {
             vis.endObject("");
           }
           hierachy.pop();
       }

       vis.close();
       return cnt;
     }
  };
}
#endif
