#include <thread>
#include <boost/program_options.hpp>
#include "janosh.hpp"
#include "commands.hpp"
#include "tcp_server.hpp"
#include "tcp_client.hpp"
#include "janosh_thread.hpp"
#include "exception.hpp"

using std::string;
using std::map;
using std::vector;
using boost::make_iterator_range;
using boost::tokenizer;
using boost::char_separator;

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

  Janosh::Janosh() :
		settings_(),
        triggers_(settings_.triggerFile, settings_.triggerDirs),
        cm_(makeCommandMap(this)) {
  }

  Janosh::~Janosh() {
  }

  void Janosh::setFormat(Format f) {
    this->format = f;
  }

  Format Janosh::getFormat() {
    return this->format;
  }

  void Janosh::open(bool readOnly=false) {
    // open the database

    uint32_t mode;
    if(readOnly)
      mode = kc::PolyDB::OAUTOTRAN | kc::PolyDB::OREADER;
    else
      mode = kc::PolyDB::OTRYLOCK | kc::PolyDB::OAUTOTRAN | kc::PolyDB::OREADER | kc::PolyDB::OWRITER | kc::PolyDB::OCREATE;
    while (!Record::db.open(settings_.databaseFile.string(),  mode)) {
			usleep(20000);
			LOG_ERR_MSG("open error: " + settings_.databaseFile.string(), Record::db.error().message());
    }
    open_ = true;
  }

  bool Janosh::isOpen() {
    return this->open_;
  }

  void Janosh::close() {
    if(isOpen()) {
      open_ = false;
      Record::db.close();
    }
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


  /**
   * Recursively traverses a record and prints it out.
   * @param rec The record to print out.
   * @param out The output stream to write to.
   * @return number of total records affected.
   */
  size_t Janosh::get(Record rec, std::ostream& out) {
    rec.fetch();
    size_t cnt = 1;

    LOG_DEBUG_MSG("get", rec.path());

    if(!rec.exists()) {
      throw janosh_exception() << record_info({"Path not found", rec});
    }

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
          out << rec.value() << endl;
          break;
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

  /**
   * Creates a temporary record with the given type.
   * @param t the type of the record to create.
   * @return the created record
   */
  Record Janosh::makeTemp(const Value::Type& t) {
    JANOSH_TRACE({},t);
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

  /**
   * Creates an array with given size. Optional performs a bounds check.
   * @param target the array record to create.
   * @param size the size of the target array.
   * @param bounds perform bounds check if true.
   * @return 1 if successful, 0 if not
   */
  size_t Janosh::makeArray(Record target, size_t size, bool bounds) {
    JANOSH_TRACE({target}, size);
    target.fetch();
    Record base = Record(target.path().basePath());
    base.fetch();
    if(!target.isDirectory() || target.exists() || base.exists()) {
      throw janosh_exception() << record_info({"Invalid target",target});
    }

    if(bounds && !boundsCheck(target)) {
      throw janosh_exception() << record_info({"Out of array bounds",target});
    }
    changeContainerSize(target.parent(), 1);
    return Record::db.add(target.path(), "A" + lexical_cast<string>(size)) ? 1 : 0;
  }

  /**
   * Creates an object with given size.
   * @param target the object record to create.
   * @param size the size of the target object.
   * @return 1 if successful, 0 if not
   */
  size_t Janosh::makeObject(Record target, size_t size) {
    JANOSH_TRACE({target}, size);
    target.fetch();
    Record base = Record(target.path().basePath());
    base.fetch();
    if(!target.isDirectory() || target.exists() || base.exists()) {
      throw janosh_exception() << record_info({"Invalid target",target});
    }

    if(!boundsCheck(target)) {
      throw janosh_exception() << record_info({"Out of array bounds", target});
    }

    if(!target.path().isRoot())
      changeContainerSize(target.parent(), 1);
    return Record::db.add(target.path(), "O" + lexical_cast<string>(size)) ? 1 : 0;
  }


  /**
   * Creates an either and array or an object with given size.
   * @param target the directory record to create.
   * @param type either Value::Array or Value::Object.
   * @param size the size of the target directory.
   * @return 1 if successful, 0 if not
   */
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
   * @return 1 if successful, 0 if not
   */
  size_t Janosh::add(Record dest, const string& value) {
    JANOSH_TRACE({dest}, value);

    if(!dest.isValue() || dest.exists()) {
      throw janosh_exception() << record_info({"Invalid target", dest});
    }

    if(!boundsCheck(dest)) {
      throw janosh_exception() << record_info({"Out of array bounds",dest});
    }

    if(Record::db.add(dest.path(), value)) {
//      if(!dest.path().isRoot())
        changeContainerSize(dest.parent(), 1);
      return 1;
    } else {
      return 0;
    }
  }

  /**
   * Relaces a the value of a record.
   * If the destination record exists create it.
   * @param dest destination record
   * @return 1 if successful, 0 if not
   */
  size_t Janosh::replace(Record dest, const string& value) {
    JANOSH_TRACE({dest}, value);
    dest.fetch();

    if(!dest.isValue() || !dest.exists()) {
      throw janosh_exception() << record_info({"Invalid target", dest});
    }

    if(!boundsCheck(dest)) {
      throw janosh_exception() << record_info({"Out of array bounds", dest});
    }

    return Record::db.replace(dest.path(), value);
  }


  /**
   * Relaces a destination record with a source record to the new path.
   * If the destination record exists replaces it.
   * @param src source record. Points to the destination record after moving.
   * @param dest destination record.
   * @return 1 if successful, 0 if not
   */
  size_t Janosh::replace(Record& src, Record& dest) {
    JANOSH_TRACE({src, dest});
    src.fetch();
    dest.fetch();

    if(src.isRange() || !src.exists() || !dest.exists()) {
      throw janosh_exception() << record_info({"Invalid target", dest});
    }

    if(!boundsCheck(dest)) {
      throw janosh_exception() << record_info({"Out of array bounds", dest});
    }

    Record target;
    size_t r;
    if(dest.isDirectory()) {
      if(src.isDirectory()) {
        target = dest.path();
        remove(dest,false);
        r = this->copy(src,target);
      } else {
        target = dest.path().basePath();
        remove(dest, false);
        r = this->copy(src,target);
      }

      dest = target;
    } else {
      if(src.isDirectory()) {
        Record target = dest.path().asDirectory();
        Record wildcard = src.path().asWildcard();
        remove(dest, false);
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
   * @return number of copied records
   */
  size_t Janosh::move(Record& src, Record& dest) {
    JANOSH_TRACE({src, dest});
    src.fetch();
    dest.fetch();

    if(src.isRange() || !src.exists()) {
      throw janosh_exception() << record_info({"Invalid src", src});
    }

    if(dest.path().isRoot()) {
      throw janosh_exception() << record_info({"Invalid target", dest});
    }

    if(!boundsCheck(dest)) {
      throw janosh_exception() << record_info({"Out of array bounds", dest});
    }

    Record target;
    size_t r;
    if(dest.isDirectory()) {
      if(src.isDirectory())
        target = dest;
      else
        target = dest.path().basePath();

      if(dest.exists())
        remove(dest);

      r = this->copy(src,target);
      dest = target;
    } else {
      if(src.isDirectory()) {
        target = dest.parent();

        if(dest.exists())
          remove(dest);

        r = this->copy(src, target);
        dest = target;
      } else {
        r = Record::db.replace(dest.path(), src.value());
      }
    }
    remove(src);
    src = dest;

    return r;
  }


  /**
   * Sets/replaces the value of a record. If no record exists, creates the record with corresponding value.
   * @param rec The record to manipulate
   * @return 1 if successful, 0 if not
   */
  size_t Janosh::set(Record rec, const string& value) {
    JANOSH_TRACE({rec}, value);

    if(!rec.isValue()) {
      throw janosh_exception() << record_info({"Invalid target", rec});
    }

    if(!boundsCheck(rec)) {
      throw janosh_exception() << record_info({"Out of array bounds", rec});
    }

    rec.fetch();
    if (rec.exists())
      return replace(rec, value);
    else
      return add(rec, value);
  }


  /**
   * Recursivley removes a record from the database.
   * @param rec The record to remove. Points to the next record in the database after removal.
   * @return number of total records affected.
   */
  size_t Janosh::remove(Record& rec, bool pack) {
    JANOSH_TRACE({rec});
    if(!rec.isInitialized())
      return 0;
 
    size_t n = rec.fetch().getSize();
    size_t cnt = 0;

    Record parent = rec.parent();
    Path targetPath = rec.path();
    Record target(targetPath);
    parent.fetch();
    target.fetch();

    if(target.isDirectory() || rec.isRange())
      rec.step();

    for(size_t i = 0; i < n; ++i) {
       if(!rec.isDirectory()) {
         rec.remove();
         ++cnt;
       } else {
         remove(rec,false);
       }
     }

    if(target.isDirectory()) {
      target.remove();
      changeContainerSize(parent, -1);
    } else {
      changeContainerSize(parent, cnt * -1);
    }

    if(pack && parent.isArray()) {
      parent.read();
      Record child = Record(parent.path()).fetch();
      size_t left = parent.getSize();
      child.step();
      for(size_t i = 0; i < left; ++i) {
        child.fetch();

        if(child.parent().path() != parent.path()) {
          throw db_exception() << record_info({"corrupted array detected", parent});
        }
        if(child.getIndex() > i) {
          Record indexPos;

          if(child.isDirectory()) {
            indexPos = parent.path().withChild(i).asDirectory();
            copy(child, indexPos);
            remove(child, false);
          } else {
            indexPos = parent.path().withChild(i);
            copy(child, indexPos);
            child.remove();
          }
        } else {
          child.next();
        }
      }
      setContainerSize(parent, left);
    }

    rec = target;

    return cnt;
  }

  /**
   * Prints all records to cout in raw format
   * @return number of total records printed.
   */
  size_t Janosh::dump(std::ostream& out) {
    kc::DB::Cursor* cur = Record::db.cursor();
    string key,value;
    cur->jump();
    size_t cnt = 0;

    while(cur->get(&key, &value, true)) {
      out << "path:" << Path(key).pretty() <<  " value:" << value << endl;
      ++cnt;
    }
    delete cur;
    return cnt;
  }

  /**
   * Calculates a hash value over all records
   * @return number of total records hashed.
   */
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

  /**
   * Clears an initializes the database.
   * @return 1 on success, 0 on fail
   */
  size_t Janosh::truncate() {
    if(Record::db.clear())
      return Record::db.add("/!", "O" + lexical_cast<string>(0)) ? 1 : 0;
    else
      return false;
  }

  /**
   * Returns the size of a directory record
   * @param rec the directory record
   * @return 1 on success, 0 on fail
   */
  size_t Janosh::size(Record rec) {
    if(!rec.isDirectory()) {
      throw janosh_exception() << record_info({"size is limited to containers", rec});
    }

    return rec.fetch().getSize();
  }

  /**
   * Append string values from an iterator range to an array record
   * @param begin the begin iterator
   * @param end the end iterator
   * @param rec the array record
   * @return number of values appended
   */
  size_t Janosh::append(vector<string>::const_iterator begin, vector<string>::const_iterator end, Record dest) {
    JANOSH_TRACE({dest});
    dest.fetch();

    if(!dest.isArray()) {
      throw janosh_exception() << record_info({"append is limited to arrays", dest});
    }

    size_t s = dest.getSize();
    size_t cnt = 0;

    for(; begin != end; ++begin) {
      if(!Record::db.add(dest.path().withChild(s + cnt), *begin)) {
        throw janosh_exception() << record_info({"Failed to add target", dest});
      }
      ++cnt;
    }

    setContainerSize(dest, s + cnt);
    return cnt;
  }

  size_t Janosh::append(Record& src, Record& dest) {
    JANOSH_TRACE({src,dest});

    if(!dest.isDirectory()) {
      throw janosh_exception() << record_info({"append is limited to directories", dest});
    }

    src.fetch();
    dest.fetch();

    size_t n = src.getSize();
    size_t s = dest.getSize();
    size_t cnt = 0;
    string path;
    string value;

    for(; cnt < n; ++cnt) {
      if(cnt > 0)
        src.next();

      src.read();
      if(src.isAncestorOf(dest)) {
        throw janosh_exception() << record_info({"can't append an ancestor", src});
      }

      if(src.isDirectory()) {
        Record target;
        if(dest.isObject()) {
          target = dest.path().withChild(src.path().name()).asDirectory();
        } else if(dest.isArray()) {
          target = dest.path().withChild(s + cnt).asDirectory();
        } else {
          throw janosh_exception() << record_info({"can't append to a value", dest});
        }

        if(src.isAncestorOf(target)) {
          throw janosh_exception() << record_info({"can't append an ancestor", src});
        }

        if(!makeDirectory(target, src.getType())) {
          throw janosh_exception() << record_info({"failed to create directory", target});
        }

        Record wildcard = src.path().asWildcard();
        if(!append(wildcard, target)) {
          throw janosh_exception() << record_info({"failed to append values", target});
        }
      } else {
        if(dest.isArray()) {
          Path target = dest.path().withChild(s + cnt);
          if(!Record::db.add(
              target,
              src.value()
          )) {
            throw janosh_exception() << record_info({"add failed", target});
          }
        } else if(dest.isObject()) {
          Path target = dest.path().withChild(src.path().name());
          if(!Record::db.add(
              target,
              src.value()
          )) {
            throw janosh_exception() << record_info({"add failed",target});
          }
        }
      }
    }

    setContainerSize(dest, s + cnt);
    return cnt;
  }

  size_t Janosh::copy(Record& src, Record& dest) {
    JANOSH_TRACE({src,dest});

    src.fetch();
    dest.fetch();
    if(dest.exists() && !src.isRange()) {
      throw janosh_exception() << record_info({"destination exists", dest});
    }

    if(dest.isRange()) {
      throw janosh_exception() << record_info({"destination can't be a range", dest});
    }

    if(src == dest)
      return 0;

    if(!src.isValue() && !dest.isDirectory()) {
      throw janosh_exception() << record_info({"invalid target", dest});
    }

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
      throw janosh_exception() << record_info({"shift is limited to operate within one array", dest});
    }

    size_t parentSize = srcParent.getSize();
    size_t srcIndex = src.getIndex();
    size_t destIndex = dest.getIndex();

    if(srcIndex >= parentSize || destIndex >= parentSize) {
      throw janosh_exception() << record_info({"index out of bounds", src});
    }

    bool back = srcIndex > destIndex;
    Record forwardRec = src.clone().fetch();
    Record backRec = src.clone().fetch();

    if(back) {
      forwardRec.previous();
    } else {
      forwardRec.next();
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

    for(int i=0; i < abs(destIndex - srcIndex); ++i) {
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
    Record tmpDir("/tmp/.");
    remove(tmpDir);
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
    return Record::db.set(path, value) ? 1 : 0;
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

  Janosh* Janosh::getInstance() {
      if(instance_ == NULL)
        instance_ = new Janosh();

      return instance_;
  }

  Janosh* Janosh::instance_ = NULL;
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

void printUsage() {
    std::cerr << "janosh [options] <command> <paths...>" << endl
        << endl
        << "Options:" << endl
        << "  -v                enable verbose output" << endl
        << "  -j                output json format" << endl
        << "  -b                output bash format" << endl
        << "  -t                execute triggers for corresponding paths" << endl
        << "  -e <target list>  execute given targets" << endl
        << endl
        << "Commands: " << endl
        <<  "  load" << endl
        <<  "  set"  << endl
        <<  "  add" << endl
        <<  "  replace" << endl
        <<  "  append" << endl
        <<  "  dump" << endl
        <<  "  size" << endl
        <<  "  get"  << endl
        <<  "  copy" << endl
        <<  "  remove" << endl
        <<  "  shift" << endl
        <<  "  move" << endl
        <<  "  truncate" << endl
        <<  "  mkarr" << endl
        <<  "  mkobj" << endl
        <<  "  hash" << endl
        << endl;
      exit(0);
}

using namespace std;
using namespace boost;
using namespace janosh;
namespace po = boost::program_options;

void handleSigInt(int s) {
  if(TcpServer::getInstance()->isOpen()) {
    LOG_DEBUG_MSG("Shutting down tcp server due to sigint", s);
    TcpServer::getInstance()->close();
  }
}

void registerSigIntHandler() {
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = handleSigInt;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
}

_INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
  _START_EASYLOGGINGPP(0, (const char**)NULL);
  registerSigIntHandler();
  try {
    string targetList;
    string command;
    vector<string> arguments;

    po::options_description genericDesc("Options");
    genericDesc.add_options()
      ("help,h", "Produce help message")
      ("verbose,v", "Enable verbose output")
      ("daemon,d", "Run in daemon mode")
      ("stand-alone,s", "Run in stand alone mode")
      ("json,j", "Produce json output")
      ("raw,r", "Produce raw output")
      ("bash,b", "Produce bash output")
      ("triggers,t", "Execute triggers")
      ("targets,e", po::value<string>(&targetList), "Execute a comma separated list of targets");

    po::options_description hidden("Hidden options");
    hidden.add_options()
      ("command", po::value< string >(&command), "The command to execute")
      ("arguments", po::value<vector<string> >(&arguments), "Arguments passed to the command");

    po::positional_options_description p;
    p.add("command", 1);
    p.add("arguments", -1);

    po::options_description cmdline_options;
    cmdline_options.add(genericDesc).add(hidden);

    po::options_description visible;
    visible.add(genericDesc);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    janosh::Format f = janosh::Bash;
    bool execTriggers = vm.count("triggers");
    bool execTargets = vm.count("targets");
    bool verbose = vm.count("verbose");
    bool daemon = vm.count("daemon");
    bool single = vm.count("single");

    if((vm.count("json") && (vm.count("bash") || vm.count("raw"))) || (vm.count("bash") && vm.count("raw"))) {
      LOG_FATAL_STR("Only one format at a time may be specified");
    }

    if(vm.count("json"))
      f = janosh::Json;
    else if(vm.count("bash"))
      f = janosh::Bash;
    else if(vm.count("raw"))
      f = janosh::Raw;

    if(vm.count("daemon") && (vm.count("bash") || vm.count("raw") || vm.count("json") || execTriggers || execTargets || single)) {
      LOG_FATAL_STR("Incompatible option(s) conflicting with daemon mode detected");
    }

    if (vm.count("help")) {
        std::cerr << "Usage: janosh [options] command ...\n";
        std::cerr << visible;
        return 0;
    }

    if (daemon) {
      Logger::init(LogLevel::L_DEBUG);
      Janosh* instance = Janosh::getInstance();
      instance->open(false);
      TcpServer* server = TcpServer::getInstance();
      server->open(instance->settings_.port);
      while (true) {
        server->run();
      }
    } else {
      Logger::init(LogLevel::L_DEBUG);

      if (command.empty() && !execTargets) {
        throw janosh_exception() << msg_info("missing command");
      }

      vector<string> vecTargets;
      if (execTargets) {
        tokenizer<char_separator<char> > tok(targetList, char_separator<char>(","));
        BOOST_FOREACH (const string& t, tok) {
          vecTargets.push_back(t);
        }
      }

      Request req(f, command, arguments, vecTargets, execTriggers, verbose);
      if(!single) {
        Settings s;
        TcpClient client;
        client.connect("localhost", s.port);
        return client.run(req);
      } else {
        Janosh* instance = Janosh::getInstance();
        instance->open(false);
        JanoshThread jt(req, std::cout);
        int rc = jt.run();
        jt.join();
        return rc;
      }
    }
  } catch (janosh_exception& ex) {
    printException(ex);
    return 1;
  } catch (std::exception& ex) {
    printException(ex);
    return 1;
  }

  return 0;
}

