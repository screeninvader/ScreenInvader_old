#ifndef _JANOSH_PATH_HPP_
#define _JANOSH_PATH_HPP_

#include <stack>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/smart_ptr.hpp>
#include <kcpolydb.h>
#include "logger.hpp"
#include <bitset>

namespace janosh {
  namespace kc = kyotocabinet;
  typedef kc::DB::Cursor Cursor;

  struct Component {
      string _key;
      string _pretty;

      const string keyEncodeIndex(const size_t& n) const {
        using namespace std;
        return bitset<std::numeric_limits<size_t>::digits>(n).to_string<char, char_traits<char>, allocator<char> >();
      }

      const size_t keyDecodeIndex(const string& s) const {
        return std::bitset<std::numeric_limits<size_t>::digits>(s).to_ulong();
      }

    public:
      Component() {}

      Component(const string& c) {
        if(!c.empty()) {
          if(c.at(0) == '#') {
            size_t i = boost::lexical_cast<size_t>(c.substr(1));
            this->_key = "$" + keyEncodeIndex(i);
            this->_pretty = "#" + boost::lexical_cast<string>(i);
          } else if(c.at(0) == '$') {
            const string enc = c.substr(1);
            size_t dec = keyDecodeIndex(enc);
            this->_key = "$" + enc;
            this->_pretty = "#" + boost::lexical_cast<string>(dec);
          } else if(c.at(0) == '.'){
            this->_key = "!";
            this->_pretty = c;
          } else if(c.at(0) == '!'){
            this->_key = "!";
            this->_pretty = ".";
          } else {
            this->_key = c;
            this->_pretty = this->_key;
          }
        }
      }

      bool operator==(const Component& other) const {
        return this->key() == other.key();
      }

      bool operator!=(const Component& other) const {
        return !(*this == other);
      }

      bool operator<(const Component& other) const {
        return this->key() <  other.key();
      }

      bool isIndex() {
        return _pretty.at(0) == '#';
      }

      bool isValid() {
        return !_pretty.empty() && !_key.empty();
      }

      bool isDirectory() {
        return _pretty == ".";
      }

      bool isWildcard() {
        return _pretty == "*";
      }

      const string& key() const {
        return this->_key;
      }

      const string& pretty() const {
        return this->_pretty;
      }
    };

  class Path {
    string keyStr;
    string prettyStr;
    std::vector<Component> components;
    bool directory;
    bool wildcard;

    string compilePathString() const;
public:
    Path();
    Path(const char* path);
    Path(const string& strPath);
    Path(const Path& other);
    void update(const string& p);
    bool operator<(const Path& other) const;
    bool operator==(const string& other) const;
    bool operator==(const Path& other) const;
    bool operator!=(const string& other) const;
    bool operator!=(const Path& other) const;
    operator string() const {
      return this->key();
    }
    const string key() const;
    const string pretty() const;
    const bool isWildcard() const;
    const bool isDirectory() const;
    bool isEmpty() const;
    bool isRoot() const;
    Path asDirectory() const;
    Path asWildcard() const;
    Path withChild(const Component& c) const;
    Path withChild(const size_t& i) const;
    void pushMember(const string& name);
    void pushIndex(const size_t& index);
    void pop(bool doUpdate=false);
    Path basePath() const;
    Component name() const;
    size_t parseIndex() const;
    Path parent() const;
    Component parentName() const;
    string root() const;
    void reset();
    const bool above(const Path& other) const;
  };
}

#endif /* _JANOSH_PATH_HPP_ */
