/*
 * component.cpp
 *
 *  Created on: Feb 16, 2014
 *      Author: elchaschab
 */

#include <boost/lexical_cast.hpp>
#include <bitset>
#include "component.hpp"

namespace janosh {

Component::Component() {
}

Component::Component(const string& c) {
  if (!c.empty()) {
    if (c.at(0) == '#') {
      size_t i = boost::lexical_cast<size_t>(c.substr(1));
      this->_key = "$" + keyEncodeIndex(i);
      this->_pretty = "#" + boost::lexical_cast<string>(i);
    } else if (c.at(0) == '$') {
      const string enc = c.substr(1);
      size_t dec = keyDecodeIndex(enc);
      this->_key = "$" + enc;
      this->_pretty = "#" + boost::lexical_cast<string>(dec);
    } else if (c.at(0) == '.') {
      this->_key = "!";
      this->_pretty = c;
    } else if (c.at(0) == '!') {
      this->_key = "!";
      this->_pretty = ".";
    } else {
      this->_key = c;
      this->_pretty = this->_key;
    }
  }
}

const string Component::keyEncodeIndex(const size_t& n) const {
  using namespace std;
  return bitset<std::numeric_limits<size_t>::digits>(n).to_string<char, char_traits<char>, allocator<char> >();
}

const size_t Component::keyDecodeIndex(const string& s) const {
  return std::bitset<std::numeric_limits<size_t>::digits>(s).to_ulong();
}

bool Component::operator==(const Component& other) const {
  return this->key() == other.key();
}

bool Component::operator!=(const Component& other) const {
  return !(*this == other);
}

bool Component::operator<(const Component& other) const {
  return this->key() < other.key();
}

bool Component::isIndex() {
  return _pretty.at(0) == '#';
}

bool Component::isValid() {
  return !_pretty.empty() && !_key.empty();
}

bool Component::isDirectory() {
  return _pretty == ".";
}

bool Component::isWildcard() {
  return _pretty == "*";
}

const string& Component::key() const {
  return this->_key;
}

const string& Component::pretty() const {
  return this->_pretty;
}
} /* namespace janosh */
