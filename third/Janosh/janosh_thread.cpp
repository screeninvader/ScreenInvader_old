/*
 * janosh_thread.cpp
 *
 *  Created on: Feb 2, 2014
 *      Author: elchaschab
 */

#include "janosh_thread.hpp"
#include "janosh.hpp"
#include "commands.hpp"
#include <thread>
#include "exception.hpp"

namespace janosh {

JanoshThread::JanoshThread(Request& req, ostream& out) :
    req_(req),
    out_(out),
    thread_(NULL) {
}

JanoshThread::~JanoshThread() {
  if(thread_ != NULL)
    delete thread_;
}

void JanoshThread::join() {
  if(thread_ != NULL)
    thread_->join();
}

int JanoshThread::run() {
  try {
    Janosh* instance = Janosh::getInstance();
    instance->setFormat(req_.format_);

    if (!req_.command_.empty()) {
      LOG_DEBUG_MSG("Execute command", req_.command_);
      Command* cmd = instance->cm_[req_.command_];

      if (!cmd) {
        throw janosh_exception() << string_info( { "Unknown command", req_.command_ });
      }

      Command::Result r = (*cmd)(req_.vecArgs_, out_);
      if (r.first == -1)
        throw janosh_exception() << msg_info(r.second);

      LOG_INFO_MSG(r.second, r.first);
    } else if (req_.vecTargets_.empty()) {
      throw janosh_exception() << msg_info("missing command");
    }

    thread_ = new std::thread([=](){
      if (req_.runTriggers_) {
        LOG_DEBUG("Triggers");
        Command* t = Janosh::getInstance()->cm_["trigger"];
        (*t)(req_.vecArgs_, out_);
      }

      if (!req_.vecTargets_.empty()) {
        LOG_DEBUG("Targets");
        Command* t = Janosh::getInstance()->cm_["target"];
        (*t)(req_.vecTargets_, out_);
      }
    });
  } catch (janosh_exception& ex) {
    printException(ex);
    return 1;
  } catch (std::exception& ex) {
    printException(ex);
    return 1;
  }

  return 0;
}
} /* namespace janosh */
