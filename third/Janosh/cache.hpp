/*
 * cache.hpp
 *
 *  Created on: Feb 3, 2014
 *      Author: elchaschab
 */

#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <stddef.h>
#include <mutex>

namespace janosh {

class Cache {
  bool valid_;
  size_t len_;
  char* data_;
  std::mutex mutex_;
  Cache();
  static Cache* instance_;
public:
  static Cache* getInstance();

  ~Cache();

  void lock();
  void unlock();
  void invalidate();
  bool isValid();
  void setData(const char * data, const size_t& len);
  char* getData();
  size_t getSize();
};

} /* namespace janosh */

#endif /* CACHE_HPP_ */
