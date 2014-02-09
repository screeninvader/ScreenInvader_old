/*
 * trigger_base.cpp
 *
 *  Created on: Feb 9, 2014
 *      Author: elchaschab
 */

#include "trigger_base.hpp"
#include <exception>
#include <sstream>
#include "json_spirit/json_spirit.h"

namespace janosh {
namespace js = json_spirit;
using std::exception;
using std::istringstream;

TriggerBase::TriggerBase(const fs::path& config, const vector<fs::path>& targetDirs) :
  targetDirs(targetDirs) {
  try {
    load(config);
  } catch (std::exception& ex) {
    LOG_FATAL_MSG("Loading the trigger base failed", ex.what());
  }
}

int TriggerBase::executeTarget(const string& name, std::ostream& out) {
  auto it = targets.find(name);
  if(it == targets.end()) {
    warn("Target not found", name);
    return -1;
  }
  LOG_INFO_MSG("Execute target", name);

    FILE* pipe = popen((*it).second.c_str(), "r");
    if (!pipe)
      LOG_ERR_MSG("Unable to execute", (*it).second);

    char buffer[1024];
    int size;
    //TIME_START
    while(!feof(pipe)) {
        size=(int)fread(buffer,1,1024, pipe); //cout<<buffer<<" size="<<size<<endl;
        out.write(buffer, size);
    }
    //TIME_PRINT_
    return pclose(pipe);
}

void TriggerBase::executeTrigger(const Path& p, std::ostream& out) {
  auto it = triggers.find(p);
  if(it != triggers.end()) {
    for(const string& name: (*it).second) {
      executeTarget(name, out);
    }
  }
}

bool TriggerBase::findAbsoluteCommand(const string& cmd, string& abs) {
  bool found = false;
  string base;
  istringstream(cmd) >> base;

  for(const fs::path& dir : targetDirs) {
    fs::path entryPath;
    fs::directory_iterator end_iter;

    for(fs::directory_iterator dir_iter(dir);
        !found && (dir_iter != end_iter); ++dir_iter) {

      entryPath = (*dir_iter).path();
      if (entryPath.filename().string() == base) {
        found = true;
        abs = dir.string() + cmd;
      }
    }
  }

  return found;
}

void TriggerBase::load(const fs::path& config) {
  if(!fs::exists(config)) {
    error("Trigger config doesn't exist", config);
  }

  ifstream is(config.string().c_str());
  load(is);
  is.close();
}

void TriggerBase::load(std::ifstream& is) {
  try {
    js::Value triggerConf;
    js::read(is, triggerConf);

    for(js::Pair& p : triggerConf.get_obj()) {
      string name = p.name_;
      js::Array arrCmdToTrigger = p.value_.get_array();

      if(arrCmdToTrigger.size() < 2){
        error("Illegal target", name);
      }

      string cmd = arrCmdToTrigger[0].get_str();
      js::Array arrTriggers = arrCmdToTrigger[1].get_array();
      string abs;

      if(!findAbsoluteCommand(cmd, abs)) {
        error("Target not found in search path", cmd);
      }

      targets[name] = abs;

      for(const js::Value& v : arrTriggers) {
        triggers[v.get_str()].insert(name);
      }
    }
  } catch (exception& e) {
    error("Unable to load trigger configuration", e.what());
  }
}


} /* namespace janosh */
