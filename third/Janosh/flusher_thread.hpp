#ifndef FLUSHER_THREAD_HPP_
#define FLUSHER_THREAD_HPP_

#include <boost/asio.hpp>
#include "request.hpp"
#include "janosh_thread.hpp"
#include "shared_pointers.hpp"

namespace janosh {

using boost::asio::ip::tcp;

class FlusherThread : public JanoshThread {
  socket_ptr socket_;
  streambuf_ptr out_buf_;
  bool cacheable_;
public:
  FlusherThread(socket_ptr s, streambuf_ptr out_buf, bool cacheable);
  void run();
};
} /* namespace janosh */

#endif /* FLUSHER_THREAD_HPP_ */
