/*
 * util.hpp
 *
 *  Created on: Feb 20, 2014
 *      Author: elchaschab
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <unistd.h>
#include <string>
#include <fstream>

namespace janosh {

using std::string;
using std::ifstream;

struct ProcessInfo {
  string cmdline_;
  __pid_t pid_;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
      ar & cmdline_;
      ar & pid_;
  }
};

ProcessInfo get_process_info(__pid_t pid);
ProcessInfo get_parent_info();
} /* namespace janosh */

#endif /* UTIL_HPP_ */
