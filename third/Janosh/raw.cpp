/*
 * raw.cpp
 *
 *  Created on: Feb 16, 2014
 *      Author: elchaschab
 */

#include "raw.hpp"

namespace janosh {
RawPrintVisitor::RawPrintVisitor(std::ostream& out) :
    PrintVisitor(out) {
}

void RawPrintVisitor::record(const Path& p, const string& value, bool array, bool first) {
  string stripped = value;
  replace(stripped.begin(), stripped.end(), '\n', ' ');
  out << stripped << std::endl;
}
} /* namespace janosh */
