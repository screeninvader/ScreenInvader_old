/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <initializer_list>
#include <assert.h>
#include "easylogging++.h"
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace janosh {
  using std::string;

  enum LogLevel {
    L_FATAL = 0,
    L_ERROR = 1,
    L_WARNING = 2,
    L_INFO = 3,
    L_DEBUG = 4,
    L_ULTRA = 5
  };

  class Record;

  class Logger {
  private:
    static const string makeCallString(const string& caller, std::initializer_list<janosh::Record> records);
  public:
    LogLevel level;

    static void init(const LogLevel l);
    static Logger& getInstance();
    static LogLevel getLevel();

    static void trace(const string& caller, std::initializer_list<janosh::Record> records);

    template<typename Tvalue>
    static void trace(const string& caller, std::initializer_list<janosh::Record> records, Tvalue value) {
      LOG(TRACE) << makeCallString(caller, records), boost::lexical_cast<string>(value);
    }

  private:
    Logger(const LogLevel l) : level(l) {}
    static Logger* instance;
  };

  #define LOG_GLOBA_STR(x) LOG(GLOBAL) << x
  #define LOG_DEBUG_STR(x) LOG(DEBUG) << x
  #define LOG_INFO_STR(x) LOG(INFO) << x
  #define LOG_WARN_STR(x) LOG(WARNING) << x
  #define LOG_ERR_STR(x) LOG(ERROR) << x
  #define LOG_FATAL_STR(x) LOG(FATAL) << x;
  #define LOG_GLOBAL_MSG(msg,x) LOG(GLOBAL) << msg << ":" << x;
  #define LOG_DEBUG_MSG(msg,x) LOG(DEBUG) << msg << ":" << x;
  #define LOG_INFO_MSG(msg,x) LOG(INFO) << msg << ":" << x;
  #define LOG_WARN_MSG(msg,x) LOG(WARNING) << msg << ":" << x;
  #define LOG_ERR_MSG(msg,x) LOG(ERROR) << msg << ":" << x;
  #define LOG_FATAL_MSG(msg,x) LOG(FATAL) << msg << ":" << x;
  #define LOG_GLOBAL(x) LOG(GLOBAL) << x
  #define LOG_DEBUG(x) LOG(DEBUG) << x
  #define LOG_INFO(x) LOG(INFO) << x
  #define LOG_WARN(x) LOG(WARNING) << x
  #define LOG_ERR(x) LOG(ERROR) << x
  #define LOG_FATAL(x) LOG(FATAL) << x;

  #define JANOSH_TRACE(...) Logger::trace(string(__FUNCTION__), ##__VA_ARGS__);
}

#endif /* LOGGER_H_ */
