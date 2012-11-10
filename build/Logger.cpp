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
 * GNU General Public License for more de0tails.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Logger.hpp"
#include "record.hpp"
#include <iostream>
#include <sstream>

namespace janosh {
  Logger* Logger::instance = NULL;

  void Logger::init(const LogLevel l) {
    Logger::instance = new Logger(l);
  }

  Logger& Logger::getInstance() {
    assert(Logger::instance != NULL);
    return *Logger::instance;
  }

  LogLevel Logger::getLevel() {
    return Logger::getInstance().level;
  }

  void Logger::trace(const string& caller, std::initializer_list<janosh::Record> records) {
    TRI_LOG_LEVEL_STR("TRACE", makeCallString(caller, records));
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
