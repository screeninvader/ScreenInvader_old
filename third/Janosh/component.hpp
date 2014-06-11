/*
 * component.hpp
 *
 *  Created on: Feb 16, 2014
 *      Author: elchaschab
 */

#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

#include <string>

namespace janosh {
using std::string;

struct Component {

  string _key;
  string _pretty;
  const string keyEncodeIndex(const size_t& n) const;
  const size_t keyDecodeIndex(const string& s) const;
public:
  Component();
  Component(const string& c);
  bool operator==(const Component& other) const;
  bool operator!=(const Component& other) const;
  bool operator<(const Component& other) const;
  bool isIndex();
  bool isValid();
  bool isDirectory();
  bool isWildcard();
  const string& key() const ;
  const string& pretty() const;
};
} /* namespace janosh */

#endif /* COMPONENT_HPP_ */
