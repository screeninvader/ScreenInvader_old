/*
 * janosh_thread.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: elchaschab
 */

#include "janosh_thread.hpp"
#include "logger.hpp"
#include "exception.hpp"
#include <thread>

namespace janosh {

using std::thread;

JanoshThread::JanoshThread(const string& name) : name_(name), result_(false) {
}

JanoshThread::~JanoshThread() {
}

bool JanoshThread::result() {
  return result_;
}

void JanoshThread::setResult(bool r) {
  result_ = r;
}

void JanoshThread::runSynchron() {
    JanoshThreadPtr holder(this);
    try {
      this->run();
    } catch(std::exception& ex) {
      janosh::printException(ex);
    }
}

void JanoshThread::runAsynchron() {
  thread t([&](){
    JanoshThreadPtr holder(this);
    Logger::registerThread(name_);
    try {
      this->run();
    } catch(std::exception& ex) {
      janosh::printException(ex);
    }
    Logger::removeThread();
  });
  t.detach();
}

} /* namespace janosh */
