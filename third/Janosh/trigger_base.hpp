/*
 * trigger_base.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: elchaschab
 */

#ifndef TRIGGER_BASE_HPP_
#define TRIGGER_BASE_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <boost/filesystem.hpp>
#include "logger.hpp"
#include "path.hpp"

namespace janosh {
namespace fs = boost::filesystem;
using std::string;
using std::ostream;
using std::ifstream;
using std::vector;
using std::set;
using std::map;

class TriggerBase {
  typedef std::map<string,string> TargetMap;
  typedef typename TargetMap::value_type Target;

  vector<fs::path> targetDirs;
  map<Path, std::set<string> > triggers;
  TargetMap targets;
public:
  TriggerBase(const fs::path& config, const vector<fs::path>& targetDirs);

  int executeTarget(const string& name, std::ostream& out);
  void executeTrigger(const Path& p, std::ostream& out);

  bool findAbsoluteCommand(const string& cmd, string& abs);
  void load(const fs::path& config);
  void load(std::ifstream& is);
  template<typename T> void error(const string& msg, T t, int exitcode=1) {
    LOG_ERR_MSG(msg, t);
    exit(exitcode);
  }

  template<typename T> void warn(const string& msg, T t) {
    LOG_WARN_MSG(msg, t);
  }
};

} /* namespace janosh */

#endif /* TRIGGER_BASE_HPP_ */
