#ifndef _JANOSH_DBPATH_HPP
#define _JANOSH_DBPATH_HPP

#include <boost/smart_ptr.hpp>
#include <kcpolydb.h>
#include <string>
#include <iostream>

#include "path.hpp"
#include "value.hpp"
#include "logger.hpp"

namespace janosh {
  namespace kc = kyotocabinet;
  typedef kc::DB::Cursor Cursor;

  class Record : private boost::shared_ptr<kc::DB::Cursor> {
    Path pathObj;
    Value valueObj;
    bool doesExist;

    void init(const Path path);
  public:
    static kc::TreeDB db;

    //exact copy referring to the same Cursor*
    Record(const Record& other);
    Record clone();
    Record(const Path& path);
    Record();


    kc::DB::Cursor* getCursorPtr();
    const Value::Type getType()  const;
    const size_t getSize() const;
    const size_t getIndex() const;

    bool get(string& k, string& v);
    bool setValue(const string& v);

    void clear();

    bool jump(const Path& p);
    bool jump_back(const Path& p);
    bool step();
    bool step_back();
    void next();
    void previous();
    void remove();

    const Path& path() const;
    const Value& value() const;
    Record parent() const;


    const bool isAncestorOf(const Record& other) const;
    const bool isArray() const;
    const bool isDirectory() const;
    const bool isRange() const;
    const bool isValue() const;
    const bool isObject() const;
    const bool isInitialized() const;
    const bool hasData() const;
    const bool exists() const;
    const bool empty() const;

    Record& fetch();
    bool readValue();
    bool readPath();
    bool read();


    bool operator==(const Record& other) const;
    bool operator!=(const Record& other) const;
  };

  std::ostream& operator<< (std::ostream& os, const janosh::Path& p);
  std::ostream& operator<< (std::ostream& os, const janosh::Value& v);
  std::ostream& operator<< (std::ostream& os, const janosh::Record& r);
}
#endif
