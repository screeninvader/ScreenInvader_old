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

#include "logger.hpp"
#include "record.hpp"
#include <iostream>
#include <sstream>

namespace el {
  namespace base {
    ThreadNameLookup* ThreadNameLookup::instance_ = NULL;
  }
}
namespace janosh {
  Logger* Logger::instance_ = NULL;


  Logger::Logger(const LogLevel l) : tracing_(false), dblog_(false), level_(l) {
    el::Configurations defaultConf;
    defaultConf.setToDefault();

    defaultConf.set(el::Level::Info, el::ConfigurationType::Format,
        "%thread %datetime %msg\t(%loc)");
    defaultConf.set(el::Level::Fatal, el::ConfigurationType::Format,
        "%thread %datetime %level %msg\t(%loc)");
    defaultConf.set(el::Level::Debug, el::ConfigurationType::Format,
        "%thread %datetime %level %msg\t(%loc)");

    // To set GLOBAL configurations you may use
    defaultConf.setGlobally(el::ConfigurationType::Format, "%thread %datetime %level %msg\t(%loc)");
    el::Loggers::reconfigureLogger("default", defaultConf);
  }

  DBLogger::~DBLogger() {
    _assert_(true);
  }

  void DBLogger::log(const char* file, int32_t line, const char* func, Kind kind, const char* message) {
    std::stringstream ss;
    ss << "DB: " << "(" << file << ":" << line << " in " << func << "):" << message;

    switch (kind) {
    case DEBUG:
      LOG(DEBUG) << ss.str();
      break;
      case INFO:
      LOG(INFO) << ss.str();
      break;
      case WARN:
      LOG(WARNING) << ss.str();
      break;
      case ERROR:
      LOG(ERROR) << ss.str();
      break;
    }
  }

  void Logger::init(const LogLevel l) {
    Logger::instance_ = new Logger(l);
  }

  Logger& Logger::getInstance() {
    assert(Logger::instance_ != NULL);
    return *Logger::instance_;
  }

  LogLevel Logger::getLevel() {
    return Logger::getInstance().level_;
  }

  bool Logger::isTracing() {
    return Logger::getInstance().tracing_;
  }

  void Logger::setTracing(bool t) {
    Logger::getInstance().tracing_ = t;
  }

  bool Logger::isDBLogging() {
    return Logger::getInstance().dblog_;
  }

  void Logger::setDBLogging(bool enabled) {
    Logger::getInstance().dblog_ = enabled;
    Record::db.tune_logger(&Logger::getInstance().dblogger_, kc::BasicDB::Logger::DEBUG
        | kc::BasicDB::Logger::INFO
        | kc::BasicDB::Logger::WARN
        | kc::BasicDB::Logger::ERROR);
  }

  void Logger::registerThread(const string& name) {
    el::base::ThreadNameLookup::set(std::this_thread::get_id(),name);
    LOG_DEBUG_MSG("Register thread", el::base::ThreadNameLookup::size());
  }

  void Logger::removeThread() {
    LOG_DEBUG("Remove thread");
    el::base::ThreadNameLookup::remove(std::this_thread::get_id());
  }

  void Logger::trace(const string& caller, std::initializer_list<janosh::Record> records) {
    LOG(TRACE) << makeCallString(caller, records);
  }

  const string Logger::makeCallString(const string& caller, std::initializer_list<janosh::Record> records) {
    std::stringstream ss;

    for(auto& rec : records) {
      ss << "\"" << rec.path().pretty() << "\",";
    }

    string arguments = ss.str();
    return caller + "(" + arguments.substr(0, arguments.size() - 1) + ")";
  }
}
