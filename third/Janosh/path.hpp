#ifndef _JANOSH_PATH_HPP_
#define _JANOSH_PATH_HPP_

#include <string>
#include <vector>
#include <kcpolydb.h>

#include "logger.hpp"
#include "component.hpp"

namespace janosh {
  using std::string;
  using std::vector;

  namespace kc = kyotocabinet;
  typedef kc::DB::Cursor Cursor;

  class Path {
    string keyStr;
    string prettyStr;
    vector<Component> components;
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
