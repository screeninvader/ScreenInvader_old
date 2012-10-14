#ifndef _JANOSH_DBPATH_HPP
#define _JANOSH_DBPATH_HPP

#include <stack>
#include <sstream>
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/smart_ptr.hpp>
#include <kcpolydb.h>
#include "Logger.hpp"

using std::string;

namespace janosh {
  namespace kc = kyotocabinet;
  enum EntryType {
    Array,
    Object,
    Value
  };


  class DBPath {
    string keyStr;
    string valueStr;

    bool hasKey;
    bool hasValue;

    std::vector<string> components;
    bool container;
    bool wildcard;
    bool doesExist;

    string compilePathString() const {
      std::stringstream ss;

      for (auto it = this->components.begin(); it != this->components.end(); ++it) {
        ss << *it;
      }
      return ss.str();
    }
  public:
    static kc::TreeDB db;

    class Cursor : private boost::shared_ptr<kc::DB::Cursor> {
      typedef  boost::shared_ptr<kc::DB::Cursor> parent;

      kc::DB::Cursor* getCursorPtr() {
        return parent::operator->();
      }
    public:
      Cursor() :
        parent()
      {};

      Cursor(kc::DB::Cursor* cur) : parent(cur) {
      }

      Cursor(const DBPath& path) :
        parent(DBPath::db.cursor()) {

        if(path.isWildcard()) {
          parent::operator->()->jump(path.basePath());
          parent::operator->()->step();
        } else if(path.isRoot()) {
          parent::operator->()->jump();
        } else {
          parent::operator->()->jump(path.key());
        }
      }

      bool isValid() {
        return (*this) != NULL;
      }

      bool remove() {
        assert(isValid());
        return getCursorPtr()->remove();
      }

      bool setValue(const string& v) {
        assert(isValid());
        return getCursorPtr()->set_value_str(v);
      }

      bool get(string& k, string& v) {
        assert(isValid());
        return getCursorPtr()->get(&k,&v);
      }

      bool jump(const string& k) {
        assert(isValid());
        if(getCursorPtr()->jump(k)) {
          string rkey;
          getKey(rkey);
          return rkey == k;
        } else
          return false;
      }

      bool jump_back(const string& k) {
        assert(isValid());
        return getCursorPtr()->jump_back(k);
      }

      bool getValue(string& v) {
        assert(isValid());
        return getCursorPtr()->get_value(&v);
      }

      bool getKey(string& v) {
        assert(isValid());
        return getCursorPtr()->get_key(&v);
      }

      bool step() {
        assert(isValid());
        return getCursorPtr()->step();
      }

      bool step_back() {
        assert(isValid());
        return getCursorPtr()->step_back();
      }

      bool next() {
        assert(isValid());
        DBPath p(*this);

        if(p.isContainer()) {
          bool success = this->step();
          size_t s = p.getSize();

          for(size_t i = 0; success && i < s; ++i) {
            success &= this->step();
          }

          LOG_DEBUG_MSG("next", p.key() + "->" + DBPath(*this).key());
          return success;
        } else {
          bool success = this->step();
          LOG_DEBUG_MSG("next", p.key() + "->" + DBPath(*this).key());
          return success;
        }
      }

      bool previous() {
        assert(isValid());
        DBPath p(*this);
        DBPath parent = p.parent();

        DBPath prev;
        DBPath prevParent;

        do {
          if(!this->step_back())
            return false;
          prev = *this;
          prevParent = prev.parent();
        } while(prevParent != parent && parent.above(prev));
        return true;
      }
    };

    DBPath(const char* path) :
        hasKey(false),
        hasValue(false),
        container(false),
        wildcard(false),
        doesExist(false) {
      update(path);
    }

    DBPath(const string& strPath) :
        hasKey(false),
        hasValue(false),
        container(false),
        wildcard(false),
        doesExist(false) {
      update(strPath);
    }

    DBPath(Cursor& cur):
        hasKey(false),
        hasValue(false),
        container(false),
        wildcard(false),
        doesExist(false) {
      read(cur);
    }

    DBPath() :
      hasKey(false),
      hasValue(false),
      container(false),
      wildcard(false),
      doesExist(false){
    }

    DBPath(const DBPath& other) {
      keyStr = other.keyStr;
      valueStr = other.valueStr;
      hasKey = other.hasKey;
      hasValue = other.hasValue;
      container = other.container;
      wildcard = other.wildcard;
      doesExist = other.doesExist;
      components = other.components;
    }

    void clear() {
      this->keyStr.clear();
      this->valueStr.clear();
      this->hasKey=false;
      this->hasValue=false;
      this->container=false;
      this->wildcard=false;
      this->container=false;
      this->components.clear();
    }

    void update(const string& p) {
      using namespace boost;
      if(p.empty()) {
        this->clear();
        return;
      }

      if(p.at(0) != '/') {
        LOG_ERR_MSG("Illegal Path", p);
        exit(1);
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
            LOG_ERR_MSG("Illegal Path", p);
            exit(1);
          }
          this->components.push_back('/' + c);
        }

        this->keyStr = p;
        this->hasKey = true;
        this->container = !this->components.empty() && this->components.back() == "/.";
        this->wildcard =  !this->components.empty() && this->components.back() == "/*";
      } else {
        clear();
      }
    }

    const bool isContainer() const {
      return this->container;
    }

    const bool isArray() const {
      return this->getType() == Array;
    }

    const bool isValue() const {
      return !this->isContainer() || this->getType() == Value;
    }

    const bool isObject() const {
      return this->getType() == Object;
    }

    const bool isWildcard() const {
      return this->wildcard;
    }

    const bool isComplete() const {
      return hasValue;
    }

    const bool exists() const {
      return this->doesExist;
    }

    const bool empty() const {
      return !hasKey;
    }

    const string& val() const {
      assert(isComplete());
      return this->valueStr;
    }

    DBPath prune(Cursor cur = Cursor()) {
      if(!isComplete()) {
        if(!read(getCursor(cur)).isValid()) {
          this->doesExist = false;
        }
      }

      return *this;
    }

    Cursor getCursor(Cursor cur = Cursor()) {
      if(this->empty()) {
        return Cursor();
      } else if(!cur.isValid()) {
        cur = Cursor(DBPath::db.cursor());
      }

      if(this->isWildcard()) {
        if(!cur.jump(this->basePath() + "/."))
          return Cursor();

        if(!cur.step())
          return Cursor();
      } else {
        if(!cur.jump(this->key()))
          return Cursor();
      }

      return cur;
    }

    DBPath makeDirectoryPath() {
      return DBPath(this->basePath() + "/.");
    }

    DBPath makeWildcardPath() {
      return DBPath(this->basePath() + "/*");
    }

    DBPath makeChildPath(const string& name) {
      return DBPath(this->basePath() + '/' + name);
    }

    DBPath makeChildPath(const size_t& i) {
      return DBPath(this->basePath() + '/' + boost::lexical_cast<string>(i));
    }

    Cursor read(Cursor cur = Cursor()) {
      if(this->empty()) {
        if(!cur.isValid()) {
          return Cursor();
        }
        string k;
        if(cur.get(k,valueStr)) {
          this->hasValue = true;
          this->doesExist = true;
          update(k);
        } else {
          this->hasKey = false;
          this->hasValue = false;
          this->doesExist = false;
          return Cursor();
        }
      } else {
        if (!cur.isValid()) {
          string k;
          cur = Cursor(DBPath::db.cursor());
          cur.jump(this->keyStr);
          cur.getKey(k);
          if (k == this->keyStr && cur.getValue(this->valueStr)) {
            this->hasValue = true;
            this->doesExist = true;
            update(k);
          } else {
            this->doesExist = false;
            return Cursor();
          }
        } else if (cur.getValue(this->valueStr)) {
          this->hasValue = true;
          this->doesExist = true;
        } else {
          this->doesExist = false;
          return Cursor();
        }
      }

      return cur;
    }

    bool isRoot() const {
      return this->key() == "/.";
    }

    const string& key() const {
      return this->keyStr;
    }

    bool operator<(const DBPath& other) const {
      return this->key() < other.key();
    }

    bool operator==(const DBPath& other) const {
      return this->key() == other.key();
    }

    bool operator!=(const DBPath& other) const {
      return this->key() != other.key();
    }

    void pushMember(const string& name) {
      components.push_back((boost::format("/%s") % name).str());
      update(compilePathString());
    }

    void pushIndex(const size_t& index) {
      components.push_back((boost::format("/%d") % index).str());
      update(compilePathString());
    }

    void pop(bool doUpdate=false) {
      components.erase(components.end() - 1);
      if(doUpdate) update(compilePathString());
    }

    string basePath() const {
      if(isContainer() || isWildcard()) {
        return this->keyStr.substr(0, this->keyStr.size() - 2);
      } else {
        return this->key();
      }
    }

    string name() const {
      size_t d = 1;

      if(isContainer())
        ++d;

      if(components.size() >= d)
        return *(components.end() - d);
      else
        return "";
    }

    size_t parseIndex() const {
        return boost::lexical_cast<size_t>(this->name().substr(1));
    }

    DBPath parent() const {
      DBPath parent(this->basePath());
      if(!parent.empty())
        parent.pop(false);
      parent.pushMember(".");

      return parent;
    }

    string parentName() const {
      size_t d = 2;
      if(isContainer())
        ++d;

      if(components.size() >= d)
        return *(components.end() - d);
      else
        return "";
    }

    string root() const {
      return components.front();
    }

    const EntryType getType()  const {
      if(this->isContainer()) {
        assert(isComplete());
        char c = valueStr.at(0);
        if(c == 'A') {
          return Array;
        } else if(c == 'O') {
          return Object;
        } else {
          assert(!"Unknown container descriptor");
        }
      }

      return Value;
    }

    const size_t getSize() const {
      assert(isComplete());

      if(this->isContainer()) {
        return boost::lexical_cast<size_t>(valueStr.substr(1));
      } else {
        return 0;
      }


    }

    bool above(const DBPath& other) const {
      if(other.components.size() >= this->components.size() ) {
        size_t depth = this->components.size();
        if(this->isContainer()) {
          depth--;
        }

        size_t i;
        for(i = 0; i < depth; ++i) {

          const string& tc = this->components[i];
          const string& oc = other.components[i];

          if(oc != tc) {
            return false;
          }
        }

        return true;
      }

      return false;
    }
  };

  typedef DBPath::Cursor Cursor;
}


#endif
