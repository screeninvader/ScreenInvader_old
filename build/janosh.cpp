#include "Commands.hpp"
#include <map>
#include <vector>
#include <initializer_list>
#include <boost/range.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>
#include <boost/function.hpp>
#include <algorithm>

using std::map;
using std::vector;
using boost::make_iterator_range;
using boost::tokenizer;
using boost::char_separator;
namespace jh = janosh;

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
  using boost::lexical_cast;

  TriggerBase::TriggerBase(const fs::path& config, const vector<fs::path>& targetDirs) :
    targetDirs(targetDirs) {
    load(config);
  }

  int TriggerBase::executeTarget(const string& name) {
    auto it = targets.find(name);
    if(it == targets.end()) {
      error("Target not found", name);
    }
    LOG_DEBUG_MSG("Execute target", name);
    return system((*it).second.c_str());
  }

  void TriggerBase::executeTrigger(const Path& p) {
    auto it = triggers.find(p);
    if(it != triggers.end()) {
      BOOST_FOREACH(const string& name, (*it).second) {
        executeTarget(name);
      }
    }
  }

  bool TriggerBase::findAbsoluteCommand(const string& cmd, string& abs) {
    bool found = false;
    string base;
    istringstream(cmd) >> base;

    BOOST_FOREACH(const fs::path& dir, targetDirs) {
      fs::path entryPath;
      fs::directory_iterator end_iter;

      for(fs::directory_iterator dir_iter(dir);
          !found && (dir_iter != end_iter); ++dir_iter) {

        entryPath = (*dir_iter).path();
        if (entryPath.filename().string() == base) {
          found = true;
          abs = dir.string() + cmd;
        }
      }
    }

    return found;
  }

  void TriggerBase::load(const fs::path& config) {
    if(!fs::exists(config)) {
      error("Trigger config doesn't exist", config);
      exit(1);
    }

    ifstream is(config.string().c_str());
    load(is);
    is.close();
  }

  void TriggerBase::load(std::ifstream& is) {
    try {
      js::Value triggerConf;
      js::read(is, triggerConf);

      BOOST_FOREACH(js::Pair& p, triggerConf.get_obj()) {
        string name = p.name_;
        js::Array arrCmdToTrigger = p.value_.get_array();

        if(arrCmdToTrigger.size() < 2){
          error("Illegal target", name);
        }

        string cmd = arrCmdToTrigger[0].get_str();
        js::Array arrTriggers = arrCmdToTrigger[1].get_array();
        string abs;

        if(!findAbsoluteCommand(cmd, abs)) {
          error("Target not found in search path", cmd);
        }

        targets[name] = abs;

        BOOST_FOREACH(const js::Value& v, arrTriggers) {
          triggers[v.get_str()].insert(name);
        }
      }
    } catch (exception& e) {
      error("Unable to load trigger configuration", e.what());
    }
  }


  Settings::Settings() {
    const char* home = getenv ("HOME");
    if (home==NULL) {
      error("Can't find environment variable.", "HOME");
    }
    string janoshDir = string(home) + "/.janosh/";
    fs::path dir(janoshDir);

    if (!fs::exists(dir)) {
      if (!fs::create_directory(dir)) {
        error("can't create directory", dir.string());
      }
    }

    this->janoshFile = fs::path(dir.string() + "janosh.json");
    this->triggerFile = fs::path(dir.string() + "triggers.json");
    this->logFile = fs::path(dir.string() + "janosh.log");

    if(!fs::exists(janoshFile)) {
      error("janosh configuration not found: ", janoshFile);
    } else {
      js::Value janoshConf;

      try{
        ifstream is(janoshFile.c_str());
        js::read(is,janoshConf);
        js::Object jObj = janoshConf.get_obj();
        js::Value v;

        if(find(jObj, "triggerDirectories", v)) {
          BOOST_FOREACH (const js::Value& vDir, v.get_array()) {
            triggerDirs.push_back(fs::path(vDir.get_str()));
          }
        }

        if(find(jObj, "database", v)) {
          this->databaseFile = fs::path(v.get_str());
        } else {

          error(this->janoshFile.string(), "No database file defined");
        }
      } catch (exception& e) {
        error("Unable to load jashon configuration", e.what());
      }
    }
  }

  bool Settings::find(const js::Object& obj, const string& name, js::Value& value) {
    auto it = find_if(obj.begin(), obj.end(),
        [&](const js::Pair& p){ return p.name_ == name;});
    if (it != obj.end()) {
      value = (*it).value_;
      return true;
    }
    return false;
  }


  Janosh::Janosh() :
    settings(),
    triggers(settings.triggerFile, settings.triggerDirs) {
  }

  void Janosh::setFormat(Format f) {
    this->format = f;
  }

  Format Janosh::getFormat() {
    return this->format;
  }

  void Janosh::open() {
    // open the database
    if (!Record::db.open(settings.databaseFile.string(), kc::PolyDB::OREADER | kc::PolyDB::OWRITER | kc::PolyDB::OCREATE)) {
      LOG_ERR_MSG("open error", Record::db.error().name());
      exit(2);
    }
  }

  void Janosh::close() {
    Record::db.close();
  }

  size_t Janosh::loadJson(const string& jsonfile) {
    std::ifstream is(jsonfile.c_str());
    size_t cnt = this->loadJson(is);
    is.close();
    return cnt;
  }

  size_t Janosh::loadJson(std::istream& is) {
    js::Value rootValue;
    js::read(is, rootValue);

    Path path;
    return load(rootValue, path);
  }

  size_t Janosh::print(Record rec, std::ostream& out) {
    rec.fetch();
    size_t cnt = 1;

    LOG_DEBUG_MSG("print", rec.path());

    if(!rec.exists())
      error("Path not found", rec.path());

    if (rec.isDirectory()) {
      switch(this->getFormat()) {
        case Json:
          cnt = recurse(rec, JsonPrintVisitor(out));
          break;
        case Bash:
          cnt = recurse(rec, BashPrintVisitor(out));
          break;
        case Raw:
          cnt = recurse(rec, RawPrintVisitor(out));
          break;
      }
    } else {
      string value;

      switch(this->getFormat()) {
        case Raw:
        case Json:
          out << rec.value() << endl;
          break;
        case Bash:
          out << "\"( [" << rec.path() << "]='" << rec.value() << "' )\"" << endl;
          break;
      }
    }
    return cnt;
  }

  Record Janosh::makeTemp(const Value::Type& t) {
    Record tmp("/tmp/.");

    if(!tmp.fetch().exists()) {
      makeDirectory(tmp, Value::Array, 0);
      tmp.fetch();
    }

    if(t == Value::String) {
      tmp = tmp.path().withChild(tmp.getSize());
      this->add(tmp, "");
    } else {
      tmp = tmp.path().withChild(tmp.getSize()).asDirectory();
      this->makeDirectory(tmp, t, 0);
    }
    return tmp;
  }

  size_t Janosh::makeArray(Record target, size_t size, bool bounds) {
    JANOSH_TRACE({target}, size);
    target.fetch();
    Record base = Record(target.path().basePath());
    base.fetch();
    assert(target.isDirectory() && !target.exists() && !base.exists());
    assert(!bounds || boundsCheck(target));

    changeContainerSize(target.parent(), 1);
    return Record::db.add(target.path(), "A" + lexical_cast<string>(size));
  }

  size_t Janosh::makeObject(Record target, size_t size) {
    JANOSH_TRACE({target}, size);
    target.fetch();
    Record base = Record(target.path().basePath());
    base.fetch();
    assert(target.isDirectory() && !target.exists() && !base.exists());
    assert(boundsCheck(target));

    if(!target.path().isRoot())
      changeContainerSize(target.parent(), 1);
    return Record::db.add(target.path(), "O" + lexical_cast<string>(size));
  }

  size_t Janosh::makeDirectory(Record target, Value::Type type, size_t size) {
    JANOSH_TRACE({target}, size);
    if(type == Value::Array) {
      return makeArray(target, size);
    } else if (type == Value::Object) {
      return makeObject(target, size);
    } else
      assert(false);
    return 0;
  }

  /**
   * Adds a record with the given value to the database.
   * Does not modify an existing record.
   * @param dest destination record
   * @return TRUE if the destination record didn't exist.
   */
  size_t Janosh::add(Record dest, const string& value) {
    JANOSH_TRACE({dest}, value);
    assert(dest.isValue() && !dest.exists());
    assert(boundsCheck(dest));

    if(Record::db.add(dest.path(), value)) {
      if(!dest.path().isRoot())
        changeContainerSize(dest.parent(), 1);
      return true;
    } else {
      return false;
    }
  }

  /**
   * Relaces a the value of a record.
   * If the destination record exists create it.
   * @param dest destination record
   * @return TRUE if the record existed.
   */
  size_t Janosh::replace(Record dest, const string& value) {
    JANOSH_TRACE({dest}, value);
    dest.fetch();
    assert(dest.isValue() && dest.exists());
    assert(boundsCheck(dest));

    return Record::db.replace(dest.path(), value);
  }


  /**
   * Relaces a destination record with a source record to the new path.
   * If the destination record exists replaces it.
   * @param src source record. Points to the destination record after moving.
   * @param dest destination record.
   * @return TRUE on success
   */
  size_t Janosh::replace(Record& src, Record& dest) {
    JANOSH_TRACE({src, dest});
    src.fetch();
    dest.fetch();

    assert(!src.isRange() && src.exists() && dest.exists());
    assert(boundsCheck(dest));
    Record target;
    size_t r;
    if(dest.isDirectory()) {
      if(src.isDirectory())
        target = dest.path();
      else
        target = dest.path().basePath();

      remove(dest);

      r = this->copy(src,target);
      dest = target;
    } else {
      if(src.isDirectory()) {
        Record target = dest.path().asDirectory();
        Record wildcard = src.path().asWildcard();
        remove(dest);
        makeDirectory(target, src.getType());
        r = this->append(wildcard, target);
        dest = target;
      } else {
        r = Record::db.replace(dest.path(), src.value());
      }
    }

    src.fetch();
    dest.fetch();

    return r;
  }

  /**
   * Copies source record value to the destination record - eventually removing the source record.
   * If the destination record exists replaces it.
   * @param src source record. Points to the destination record after moving.
   * @param dest destination record.
   * @return TRUE on success
   */
  size_t Janosh::move(Record& src, Record& dest) {
    JANOSH_TRACE({src, dest});
    src.fetch();
    dest.fetch();

    assert(!src.isRange() && src.exists() && dest.exists());
    assert(boundsCheck(dest));
    Record target;
    size_t r;
    if(dest.isDirectory()) {
      if(src.isDirectory())
        target = dest;
      else
        target = dest.path().basePath();

      remove(dest);

      r = this->copy(src,target);
      dest = target;
    } else {
      if(src.isDirectory()) {
        target = dest.parent();
        r = this->copy(src, target);
        dest = target;
      } else {
        r = Record::db.replace(dest.path(), src.value());
      }
    }


    src = dest;

    return r;
  }


  /**
   * Sets/replaces the value of a record. If no record exists, creates the record with corresponding value.
   * @param rec The record to manipulate
   * @return TRUE on success
   */
  size_t Janosh::set(Record rec, const string& value) {
    JANOSH_TRACE({rec}, value);
    assert(rec.isValue());
    assert(boundsCheck(rec));

    rec.fetch();
    if (rec.exists())
      return replace(rec, value);
    else
      return add(rec, value);
  }


  /**
   * Removes a record from the database.
   * @param rec The record to remove. Points to the next record in the database after removal.
   * @return number of total records affected.
   */
  size_t Janosh::remove(Record& rec) {
    JANOSH_TRACE({rec});
    if(!rec.isInitialized())
      return 0;
    rec.fetch();
    size_t n = rec.getSize();
    size_t cnt = 0;

    if(rec.isDirectory()) {
      Record parent = rec.parent();

      if(parent.fetch().exists())
        changeContainerSize(parent, -1);

      rec.remove();
      for(size_t i = 0; i < n; ++i) {
        if(!rec.isDirectory()) {
          rec.remove();
          ++cnt;
        } else {
          cnt += remove(rec);
        }
      }
    } else {
      for(size_t i = 0; i < n; ++i) {
        Record parent = rec.parent();
        rec.fetch();
        Value::Type rootType = rec.getType();

        if(parent.fetch().exists())
          changeContainerSize(parent, -1);

        if (rootType != Value::String) {
          const size_t size = rec.getSize();
          rec.remove();
          ++cnt;

          for (size_t j = 0; j < size; ++j) {
            if(!rec.isDirectory()) {
              rec.remove();
              ++cnt;
            } else {
              cnt += remove(rec);
            }
          }
        } else {
          rec.remove();
          ++cnt;
        }
      }
    }
    return cnt;
  }

  size_t Janosh::dump() {
    kc::DB::Cursor* cur = Record::db.cursor();
    string key,value;
    cur->jump();
    size_t cnt = 0;

    while(cur->get(&key, &value, true)) {
      std::cout << "path: " << Path(key).pretty() <<  " value:" << value << endl;
      ++cnt;
    }
    delete cur;
    return cnt;
  }

  size_t Janosh::hash() {
    kc::DB::Cursor* cur = Record::db.cursor();
    string key,value;
    cur->jump();
    size_t cnt = 0;
    boost::hash<string> hasher;
    size_t h = 0;
    while(cur->get(&key, &value, true)) {
      h = hasher(lexical_cast<string>(h) + key + value);
      ++cnt;
    }
    std::cout << h << std::endl;
    delete cur;
    return cnt;
  }

  size_t Janosh::truncate() {
    if(Record::db.clear())
      return Record::db.add("/!", "O" + lexical_cast<string>(0));
    else
      return false;
  }

  size_t Janosh::size(Record rec) {
    if(!rec.isDirectory())
      error("size is limited to containers", rec.path());

    return rec.fetch().getSize();
  }

  size_t Janosh::append(Record dest, const string& value) {
    JANOSH_TRACE({dest}, value);
    if(!dest.isDirectory())
      error("append is limited to dest directories", dest.path());

    Record target(dest.path().withChild(dest.getSize()));
    assert(Record::db.add(target.path(), value));
    dest = target;
    return 1;
  }

  size_t Janosh::append(vector<string>::const_iterator begin, vector<string>::const_iterator end, Record dest) {
    JANOSH_TRACE({dest});
    if(!dest.isDirectory())
      error("append is limited to dest directories", dest.path());
    dest.fetch();
    size_t s = dest.getSize();
    size_t cnt = 0;

    for(; begin != end; ++begin) {
      assert(Record::db.add(dest.path().withChild(s + cnt), *begin));
      ++cnt;
    }

    setContainerSize(dest, s + cnt);
    return cnt;
  }

  size_t Janosh::append(Record& src, Record& dest) {
    JANOSH_TRACE({src,dest});

    if(!dest.isDirectory())
      error("append is limited to directories", dest.path());

    src.fetch();
    dest.fetch();

    size_t n = src.isDirectory() ? 1 : src.getSize();
    size_t s = dest.getSize();
    size_t cnt = 0;

    string path;
    string value;

    do {
      src.read();
      assert(!src.isAncestorOf(dest));
      if(src.isDirectory()) {
        Record target;
        if(dest.isObject()) {
          target = dest.path().withChild(src.path().name()).asDirectory();
        } else if(dest.isArray()) {
          target = dest.path().withChild(s + cnt).asDirectory();
        } else
          assert(false);

        assert(!src.isAncestorOf(target));
        assert(makeDirectory(target, src.getType()));
        Record wildcard = src.path().asWildcard();
        assert(append(wildcard, target));
      } else {
        if(dest.isArray()) {
          assert(Record::db.add(
              dest.path().withChild(s + cnt),
              src.value()
          ));
        } else if(dest.isObject()) {
          assert(Record::db.add(
              dest.path().withChild(src.path().name()),
              src.value()
          ));
        }
      }
    } while(++cnt < n && src.next());

    setContainerSize(dest, s + cnt);
    return cnt;
  }

  size_t Janosh::copy(Record& src, Record& dest) {
    JANOSH_TRACE({src,dest});

    src.fetch();
    dest.fetch();
    if(dest.exists() && dest.isRange())
      error("Destination can't be a range", dest.path());

    if(src == dest)
      return 0;

    assert(src.isValue() || dest.isDirectory());

    if((src.isRange() || src.isDirectory()) && !dest.exists()) {
      makeDirectory(dest, src.getType());
      Record wildcard = src.path().asWildcard();
      return this->append(wildcard, dest);
    } else if (src.isValue() && dest.isValue()) {
      return this->set(dest, src.value());
    } else {
      return this->append(src, dest);
    }

    return 0;
  }

  size_t Janosh::shift(Record& src, Record& dest) {
    JANOSH_TRACE({src,dest});

    Record srcParent = src.parent();
    srcParent.fetch();

    if(!srcParent.isArray() || srcParent != dest.parent()) {
      error("Move is limited within one array", src.path().key() + "->" + dest.path().key());
    }

    size_t parentSize = srcParent.getSize();
    size_t srcIndex = src.getIndex();
    size_t destIndex = dest.getIndex();

    if(srcIndex >= parentSize || destIndex >= parentSize) {
      error("index out of bounds", src.path());
    }

    bool back = srcIndex > destIndex;
    Record forwardRec = src.clone().fetch();
    Record backRec = src.clone().fetch();

    if(back) {
      assert(forwardRec.previous());
    } else {
      assert(forwardRec.next());
    }

    Record tmp;

    if(src.isDirectory()) {
      tmp = makeTemp(src.getType());
      Record wildcard = src.path().asWildcard();
      this->append(wildcard, tmp);
    } else {
      tmp = makeTemp(Value::String);
      this->set(tmp, src.value());
    }

    for(size_t i=0; i < abs(destIndex - srcIndex); ++i) {
      replace(forwardRec, backRec);
      if(back) {
        backRec.previous();
        forwardRec.previous();
      } else {
        backRec.next();
        forwardRec.next();
      }
    }

    tmp.fetch();
    replace(tmp,dest);
    remove(tmp);

    src = dest;

    return 1;
  }

  void Janosh::setContainerSize(Record container, const size_t s) {
    JANOSH_TRACE({container}, s);

    string containerValue;
    string strContainer;
    const Value::Type& containerType = container.getType();
    char t;

    if (containerType == Value::Array)
     t = 'A';
    else if (containerType == Value::Object)
     t = 'O';
    else
     assert(false);

    const string& new_value =
       (boost::format("%c%d") % t % (s)).str();

    container.setValue(new_value);
  }

  void Janosh::changeContainerSize(Record container, const size_t by) {
    container.fetch();
    setContainerSize(container, container.getSize() + by);
  }

  size_t Janosh::load(const Path& path, const string& value) {
    LOG_DEBUG_MSG("add", path.key());
    return Record::db.set(path, value);
  }

  size_t Janosh::load(js::Value& v, Path& path) {
    size_t cnt = 0;
    if (v.type() == js::obj_type) {
      cnt+=load(v.get_obj(), path);
    } else if (v.type() == js::array_type) {
      cnt+=load(v.get_array(), path);
    } else {
      cnt+=this->load(path, v.get_str());
    }
    return cnt;
  }

  size_t Janosh::load(js::Object& obj, Path& path) {
    size_t cnt = 0;
    path.pushMember(".");
    cnt+=this->load(path, (boost::format("O%d") % obj.size()).str());
    path.pop();

    BOOST_FOREACH(js::Pair& p, obj) {
      path.pushMember(p.name_);
      cnt+=load(p.value_, path);
      path.pop();
      ++cnt;
    }

    return cnt;
  }

  size_t Janosh::load(js::Array& array, Path& path) {
    size_t cnt = 0;
    int index = 0;
    path.pushMember(".");
    cnt+=this->load(path, (boost::format("A%d") % array.size()).str());
    path.pop();

    BOOST_FOREACH(js::Value& v, array){
      path.pushIndex(index++);
      cnt+=load(v, path);
      path.pop();
      ++cnt;
    }
    return cnt;
  }

  bool Janosh::boundsCheck(Record p) {
    Record parent = p.parent();

    p.fetch();
    parent.fetch();

    return (parent.path().isRoot() || (!parent.isArray() || p.path().parseIndex() <= parent.getSize()));
  }
}

void printUsage() {
  std::cerr << "janosh [-l] <path>" << endl
      << endl
      << "<path>         the json path (uses / as separator)" << endl
      << endl
      << "Options:" << endl
      << endl
      << "-l             load a json snippet from standard input" << endl
      << endl;
  exit(1);
}

typedef map<const string, jh::Command*> CommandMap;

CommandMap makeCommandMap(jh::Janosh* janosh) {
  CommandMap cm;
  cm.insert({"load", new jh::LoadCommand(janosh) });
//  cm.insert({"dump", bind(constructor<jh::DumpCommand>(),_1) });
  cm.insert({"add", new jh::AddCommand(janosh) });
  cm.insert({"replace", new jh::ReplaceCommand(janosh) });
  cm.insert({"set", new jh::SetCommand(janosh) });
  cm.insert({"get", new jh::GetCommand(janosh) });
  cm.insert({"copy", new jh::CopyCommand(janosh) });
  cm.insert({"remove", new jh::RemoveCommand(janosh) });
  cm.insert({"shift", new jh::ShiftCommand(janosh) });
  cm.insert({"append", new jh::AppendCommand(janosh) });
  cm.insert({"dump", new jh::DumpCommand(janosh) });
  cm.insert({"size", new jh::SizeCommand(janosh) });
  cm.insert({"triggers", new jh::TriggerCommand(janosh) });
  cm.insert({"targets", new jh::TargetCommand(janosh) });
  cm.insert({"truncate", new jh::TruncateCommand(janosh) });
  cm.insert({"mkarr", new jh::MakeArrayCommand(janosh) });
  cm.insert({"mkobj", new jh::MakeObjectCommand(janosh) });
  cm.insert({"hash", new jh::HashCommand(janosh) });

  return cm;
}

std::vector<size_t> sequence() {
  std::vector<size_t> v(10);
  size_t off = 5;
  std::generate(v.begin(), v.end(), [&]() {
    return ++off;
  });
  return v;
}


kyotocabinet::TreeDB janosh::Record::db;
using namespace janosh;

int main(int argc, char** argv) {
  using namespace std;
  int c;

  janosh::Format f = janosh::Bash;

  bool verbose = false;
  bool execTriggers = false;
  bool execTargets = false;

  string key;
  string value;
  string targetList;

  while ((c = getopt(argc, argv, "vfjbrte:")) != -1) {
    switch (c) {
    case 'f':
      if(string(optarg) == "bash")
        f=janosh::Bash;
      else if(string(optarg) == "json")
        f=janosh::Json;
      else if(string(optarg) == "raw")
        f=janosh::Raw;
       else
        printUsage();
      break;
    case 'j':
      f=janosh::Json;
      break;
    case 'b':
      f=janosh::Bash;
      break;
    case 'r':
      f=janosh::Raw;
      break;
    case 'v':
      verbose=true;
      break;
    case 't':
      execTriggers = true;
      break;
    case 'e':
      execTargets = true;
      targetList = optarg;
      break;
    case ':':
      printUsage();
      break;
    case '?':
      printUsage();
      break;
    }
  }
  jh::Janosh janosh;
  janosh.setFormat(f);

  if(verbose)
    Logger::init(LogLevel::L_DEBUG);
  else
    Logger::init(LogLevel::L_INFO);

  janosh.open();
  CommandMap cm = makeCommandMap(&janosh);

  if(argc - optind >= 1) {
    string strCmd = string(argv[optind]);
    jh::Command* cmd = cm[strCmd];
    if(!cmd)
      janosh.error("Unknown command", strCmd);

    vector<string> vecParams;
    for(size_t i = optind+1; i < argc; ++i) {
      vecParams.push_back(argv[i]);
    }

    jh::Command::Result r = (*cmd)(vecParams);
    LOG_INFO_MSG(r.second, r.first);

    janosh.close();
    if(strCmd == "set" && execTriggers) {
      vector<string> vecTriggers;

      for(size_t i = optind+1; i < argc; i+=2) {
        vecTriggers.push_back(argv[i]);
      }
      (*cm["triggers"])(vecTriggers);
    }

    return r.first ? 0 : 1;
  } else if(!execTargets){
    printUsage();
  }

  janosh.close();

  if(execTargets) {
     vector<string> vecTargets;
     tokenizer<char_separator<char> > tok(targetList, char_separator<char>(","));

     BOOST_FOREACH (const string& t, tok) {
       vecTargets.push_back(t);
     }

     (*cm["targets"])(vecTargets);
   }


  return 0;
}
