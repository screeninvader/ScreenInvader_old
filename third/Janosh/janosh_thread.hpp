#ifndef JANOSH_THREAD_HPP_
#define JANOSH_THREAD_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include "format.hpp"
#include "request.hpp"

namespace janosh {

using std::string;
using std::vector;
using std::ostream;
using std::thread;

class JanoshThread {
  Request req_;
  ostream& out_;
  std::thread* thread_;
public:
  JanoshThread(Request& req, ostream& out);
  ~JanoshThread();
  void join();
  int run();
};


} /* namespace janosh */

#endif /* JANOSH_THREAD_HPP_ */
