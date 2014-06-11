/*
 * raw.hpp
 *
 *  Created on: Feb 16, 2014
 *      Author: elchaschab
 */

#ifndef RAW_HPP_
#define RAW_HPP_

#include "print_visitor.hpp"

namespace janosh {

class RawPrintVisitor : public PrintVisitor {
public:
  RawPrintVisitor(std::ostream& out);
  virtual void record(const Path& p, const string& value, bool array, bool first);
};
} /* namespace janosh */

#endif /* RAW_HPP_ */
