/*
 * janosh_thread.h
 *
 *  Created on: Feb 22, 2014
 *      Author: elchaschab
 */

#ifndef JANOSH_THREAD_H_
#define JANOSH_THREAD_H_


#include <string>
#include <memory>

namespace janosh {
using std::string;
using std::shared_ptr;

class JanoshThread {
  typedef shared_ptr<JanoshThread> JanoshThreadPtr;
  string name_;
  bool result_;
public:
  JanoshThread(const string& name);
  virtual ~JanoshThread();

  virtual void run() = 0;
  void runSynchron();
  void runAsynchron();
  bool result();
  void setResult(bool r);
};

} /* namespace janosh */

#endif /* JANOSH_THREAD_H_ */
