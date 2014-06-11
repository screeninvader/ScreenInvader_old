/*
 * cache.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: elchaschab
 */

#include "cache.hpp"
#include "logger.hpp"

namespace janosh {

Cache* Cache::instance_ = NULL;

Cache* Cache::getInstance() {
  if(instance_ == NULL)
    instance_ = new Cache();

  return instance_;
}

Cache::Cache() : valid_(false), len_(2048), data_((char*)malloc(len_)) {
  if(data_ == NULL)
    LOG_FATAL_STR("Unable to allocate cache");
}

Cache::~Cache() {
  free(data_);
}

void Cache::lock() {
  mutex_.lock();
}

void Cache::unlock() {
  mutex_.unlock();
}

void Cache::invalidate() {
  valid_ = false;
}

bool Cache::isValid() {
  return valid_;
}

void Cache::setData(const char * data, const size_t& len) {
  lock();
  if(len > len_) {
    data_ = (char*) realloc(data_, len);
    if(data_ == NULL)
      LOG_FATAL_STR("Unable to allocate cache");
  }
  len_ = len;
  memcpy(data_, data, len_);
  valid_ = true;
  unlock();
}

char* Cache::getData() {
  return data_;
}

size_t Cache::getSize() {
  return len_;
}

} /* namespace janosh */
