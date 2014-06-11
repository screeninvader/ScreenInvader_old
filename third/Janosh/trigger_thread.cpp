/*
 * janosh_thread.cpp
 *
 *  Created on: Feb 2, 2014
 *      Author: elchaschab
 */

#include "trigger_thread.hpp"
#include "commands.hpp"
#include "exception.hpp"
#include "tracker.hpp"

namespace janosh {

TriggerThread::TriggerThread(Request& req, ostream_ptr out) :
    JanoshThread("Trigger"),
    req_(req),
    out_(out) {
}

void TriggerThread::run() {
  try {
    Tracker* tracker = Tracker::getInstancePerThread();

    if(req_.runTriggers_ || !req_.vecTargets_.empty()) {
        if (req_.runTriggers_) {
          Command* t = Janosh::getInstance()->cm_["trigger"];
          map<string, size_t> keysModified = tracker->get(Tracker::WRITE);
          LOG_DEBUG("Reset tracker");
          tracker->reset();
          vector<string> triggers;
          for(auto iter : keysModified) {
            triggers.push_back(iter.first);
          }

          (*t)(triggers, *out_);
        }

        if (!req_.vecTargets_.empty()) {
          LOG_DEBUG("Targets");
          Command* t = Janosh::getInstance()->cm_["target"];
          (*t)(req_.vecTargets_, *out_);
        }

        setResult(true);
        return;
    }
  } catch (janosh_exception& ex) {
    printException(ex);
  } catch (std::exception& ex) {
    printException(ex);
  }
}
} /* namespace janosh */
