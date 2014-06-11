/*
 * tracker.hpp
 *
 *  Created on: Feb 15, 2014
 *      Author: elchaschab
 */

#ifndef TRACKER_HPP_
#define TRACKER_HPP_

#include <string>
#include <map>
#include "path.hpp"
#include "exception.hpp"
#include <iostream>
#include <thread>

namespace janosh {
using std::string;
using std::map;
using std::ostream;
using std::thread;
class Tracker {
public:
  enum PrintDirective {
    DONTPRINT,
    PRINTMETA,
    PRINTFULL
  };
private:
  map<string, size_t> reads_;
  map<string, size_t> writes_;
  map<string, size_t> deletes_;
  map<string, size_t> triggers_;
  static map<thread::id, Tracker*> instances_;
  PrintDirective printDirective_;
  void printMeta(ostream& out);
  void printFull(ostream& out);
public:
  enum Operation {
    READ,
    WRITE,
    DELETE,
    TRIGGER
  };

  Tracker() : printDirective_(DONTPRINT) {};
  virtual ~Tracker() {};

  void update(const string& s, const Operation& op);
  size_t get(const string& s, const Operation& op);
  map<string, size_t>& get(const Operation& op);
  void reset();
  void print(ostream& out);

  static Tracker* getInstancePerThread();
  static void setPrintDirective(PrintDirective p);
  static PrintDirective getPrintDirective();
};

} /* namespace janosh */

#endif /* TRACKER_HPP_ */
