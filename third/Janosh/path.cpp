#include "path.hpp"
#include "exception.hpp"

  using namespace janosh;
  using std::string;

  string Path::compilePathString() const {
    std::stringstream ss;

    for (auto it = this->components.begin(); it != this->components.end(); ++it) {
      ss << '/' << (*it).key();
    }
    return ss.str();
  }

  Path::Path() :
    directory(false),
    wildcard(false)
  {}

  Path::Path(const char* path) :
      directory(false),
      wildcard(false) {
    update(path);
  }

  Path::Path(const string& strPath) :
      directory(false),
      wildcard(false) {
    update(strPath);
  }

  Path::Path(const Path& other) {
    this->keyStr = other.keyStr;
    this->prettyStr = other.prettyStr;
    this->directory = other.directory;
    this->wildcard = other.wildcard;
    this->components = other.components;
  }

  void Path::update(const string& p) {
    using namespace boost;
    if(p.empty()) {
      this->reset();
      return;
    }

    if(p.at(0) != '/') {
      throw path_exception() << string_info({"illegal path", p});
    }

    char_separator<char> ssep("[/", "", boost::keep_empty_tokens);
    tokenizer<char_separator<char> > tokComponents(p, ssep);
    this->components.clear();
    tokComponents.begin();
    if(tokComponents.begin() != tokComponents.end()) {
      auto it = tokComponents.begin();
      it++;
      for(; it != tokComponents.end(); it++) {
        const string& c = *it;
        if(c.empty()) {
          throw path_exception() << string_info({"illegal path", p});
        }
        this->components.push_back(Component(c));
      }

      this->keyStr.clear();
      this->prettyStr.clear();
      for(auto it = this->components.begin(); it != this->components.end(); ++it) {
        this->keyStr+= "/" + (*it).key();
        this->prettyStr+= "/" + (*it).pretty();
      }

      this->directory = !this->components.empty() && this->components.back().isDirectory();
      this->wildcard = !this->components.empty() && this->components.back().isWildcard();
    } else {
      reset();
    }
  }

  bool Path::operator<(const Path& other) const {
    return this->key() < other.key();
  }

  bool Path::operator==(const string& other) const {
    return this->keyStr == other;
  }

  bool Path::operator==(const Path& other) const {
    return this->keyStr == other.keyStr;
  }

  bool Path::operator!=(const string& other) const {
    return this->keyStr != other;
  }

  bool Path::operator!=(const Path& other) const {
    return this->keyStr != other.keyStr;
  }

  const string Path::key() const {
    return this->keyStr;
  }

  const string Path::pretty() const {
    return this->prettyStr;
  }

  const bool Path::isWildcard() const {
    return this->wildcard;
  }

  const bool Path::isDirectory() const {
    return this->directory;
  }

  bool Path::isEmpty() const {
    return this->keyStr.empty();
  }

  bool Path::isRoot() const {
    return this->prettyStr == "/.";
  }

  Path Path::asDirectory() const {
    return Path(this->basePath().key() + "/!");
  }

  Path Path::asWildcard() const {
    return Path(this->basePath().key() + "/*");
  }

  Path Path::withChild(const Component& c) const{
    return Path(this->basePath().key() + '/' + c.key());
  }

  Path Path::withChild(const size_t& i) const {
    return Path(this->basePath().key() + "/#" + boost::lexical_cast<string>(i));
  }

  void Path::pushMember(const string& name) {
    components.push_back((boost::format("%s") % name).str());
    update(compilePathString());
  }

  void Path::pushIndex(const size_t& index) {
    components.push_back((boost::format("#%d") % index).str());
    update(compilePathString());
  }

  void Path::pop(bool doUpdate) {
    components.erase(components.end() - 1);
    if(doUpdate) update(compilePathString());
  }

  Path Path::basePath() const {
    if(isDirectory() || isWildcard()) {
      return Path(this->keyStr.substr(0, this->keyStr.size() - 2));
    } else {
      return Path(*this);
    }
  }

  Component Path::name() const {
    size_t d = 1;

    if(isDirectory())
      ++d;

    if(components.size() >= d)
      return (*(components.end() - d));
    else
      return Component();
  }

  size_t Path::parseIndex() const {
      return boost::lexical_cast<size_t>(this->name().pretty().substr(1));
  }

  Path Path::parent() const {
    Path parent(this->basePath());
    if(!parent.isEmpty() && !this->isWildcard())
      parent.pop(false);
    parent.pushMember(".");

    return parent;
  }

  Component Path::parentName() const {
    size_t d = 2;
    if(isDirectory())
      ++d;

    if(components.size() >= d)
      return (*(components.end() - d)).key();
    else
      return Component();
  }

  string Path::root() const {
    return components.front().key();
  }

  void Path::reset() {
    this->keyStr.clear();
    this->prettyStr.clear();
    this->components.clear();
    this->directory = false;
    this->wildcard = false;
  }

  const bool Path::above(const Path& other) const {
    if(other.components.size() >= this->components.size() ) {
      size_t depth = this->components.size();
      if(this->isDirectory()) {
        depth--;
      }

      size_t i;
      for(i = 0; i < depth; ++i) {

        const string& tc = this->components[i].key();
        const string& oc = other.components[i].key();

        if(oc != tc) {
          return false;
        }
      }

      return true;
    }

    return false;
  }

