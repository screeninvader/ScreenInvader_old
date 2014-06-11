#ifndef TRIGGER_THREAD_HPP_
#define TRIGGER_THREAD_HPP_

#include <string>
#include <vector>
#include <iostream>
#include "format.hpp"
#include "request.hpp"
#include "janosh_thread.hpp"
#include "shared_pointers.hpp"

namespace janosh {

using std::string;
using std::vector;
using std::ostream;

class TriggerThread : public JanoshThread {
  Request req_;
  ostream_ptr out_;
public:
  TriggerThread(Request& req, ostream_ptr out);
  void run();
};


} /* namespace janosh */

#endif /* TRIGGER_THREAD_HPP_ */
