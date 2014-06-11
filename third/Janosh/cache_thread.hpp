#ifndef CACHE_THREAD_HPP_
#define CACHE_THREAD_HPP_

#include <iostream>
#include <boost/asio.hpp>
#include "request.hpp"
#include "janosh_thread.hpp"
#include "shared_pointers.hpp"

namespace janosh {

using std::ostream;
using boost::asio::ip::tcp;

class CacheThread : public JanoshThread {
  socket_ptr socket_;
public:
  CacheThread(socket_ptr s);
  void run();
};
} /* namespace janosh */

#endif /* CACHE_THREAD_HPP_ */
