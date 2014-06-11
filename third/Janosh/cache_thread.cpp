/*
 * janosh_thread.cpp
 *
 *  Created on: Feb 2, 2014
 *      Author: elchaschab
 */

#include <boost/asio.hpp>

#include "cache_thread.hpp"
#include "janosh.hpp"
#include "commands.hpp"
#include "exception.hpp"
#include "cache.hpp"

namespace janosh {

CacheThread::CacheThread(socket_ptr s) :
    JanoshThread("Cache"),
    socket_(s) {
}

void CacheThread::run() {
  Cache* cache = Cache::getInstance();
  cache->lock();
  try {
    boost::asio::streambuf rc_buf;
    ostream rc_stream(&rc_buf);
    rc_stream << std::to_string(0) << '\n';
    LOG_DEBUG_MSG("sending", rc_buf.size());
    boost::asio::write(*socket_, rc_buf);
    LOG_DEBUG_MSG("sending", cache->getSize());
    boost::asio::write(*socket_, boost::asio::buffer(cache->getData(), cache->getSize()));
    setResult(true);
  } catch(std::exception& ex) {
    janosh::printException(ex);
  }
  cache->unlock();
}
} /* namespace janosh */
