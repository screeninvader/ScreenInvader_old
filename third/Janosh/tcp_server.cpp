/*
 * CheesyServer.cpp
 *
 *  Created on: Jan 6, 2014
 *      Author: elchaschab
 */

#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "tcp_server.hpp"
#include "format.hpp"
#include "logger.hpp"
#include "janosh_thread.hpp"
#include "exception.hpp"
#include "cache.hpp"


namespace janosh {

using boost::asio::ip::tcp;
using std::string;
using std::vector;
using std::stringstream;
using std::function;
using std::ostream;

TcpServer* TcpServer::instance_;

TcpServer::TcpServer() : io_service(), acceptor(io_service), cache_() {
}


void TcpServer::open(int port) {
  tcp::resolver resolver(io_service);
  tcp::resolver::query query("0.0.0.0", std::to_string(port));
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

  acceptor.open(endpoint.protocol());
  acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor.bind(endpoint);
  acceptor.listen();
}

TcpServer::~TcpServer() {
  this->close();
}

bool TcpServer::isOpen() {
  return acceptor.is_open();
}

void TcpServer::close() {
  acceptor.close();
}

void splitAndPushBack(string& s, vector<string>& vec) {
  std::size_t i;
  std::size_t lasti = 0;
  string arg;
  while((i = s.find(",", lasti)) != string::npos) {
    arg = s.substr(lasti, i - lasti);
    if(!arg.empty()) {
      vec.push_back(arg);
    }
    lasti = i + 1;
  }

  arg = s.substr(lasti, s.size());
  if(!arg.empty()) {
    vec.push_back(arg);
  }
}

string reconstructCommandLine(Format& format, string& command, vector<string>& vecArgs, vector<string>& vecTargets, bool& runTriggers, bool& verbose) {
  string cmdline = "janosh ";

  if(verbose)
    cmdline += "-v ";

  if(format == janosh::Bash)
    cmdline += "-b ";
  else if(format == janosh::Json)
    cmdline += "-j ";
  else if(format == janosh::Raw)
    cmdline += "-r ";

  if(runTriggers)
     cmdline += "-t ";

   if(!vecTargets.empty()) {
     cmdline += "-e ";
     bool first = true;
     for(const string& target : vecTargets) {
       if(!first)
         cmdline+=",";
       cmdline+=target;

       first = false;
     }
     cmdline += " ";
   }

   cmdline += (command + " ");

   if(!vecArgs.empty()) {
     bool first = true;
     for(const string& arg : vecArgs) {
       if(!first)
         cmdline+=" ";
       cmdline+=arg;

       first = false;
     }
     cmdline += " ";
   }

  return cmdline;
}

void TcpServer::run() {
	boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(io_service);
	acceptor.accept(*socket);

	try {
    Format format;
    string command;
    vector<string> vecArgs;
    vector<string> vecTargets;
    bool runTriggers;
    bool verbose;


    std::string peerAddr = socket->remote_endpoint().address().to_string();

    LOG_DEBUG_MSG("accepted", peerAddr);
    string line;
    boost::asio::streambuf response;
    boost::asio::read_until(*socket, response, "\n");
    std::istream response_stream(&response);

    std::getline(response_stream, line);

    LOG_DEBUG_MSG("format", line);
    if (line == "BASH") {
      format = janosh::Bash;
    } else if (line == "JSON") {
      format = janosh::Json;
    } else if (line == "RAW") {
      format = janosh::Raw;
    } else
      throw janosh_exception() << string_info( { "Illegal formats line", line });

    std::getline(response_stream, command);
    LOG_DEBUG_MSG("command", command);

    std::getline(response_stream, line);
    LOG_DEBUG_MSG("args", line);

    splitAndPushBack(line, vecArgs);

    std::getline(response_stream, line);
    LOG_DEBUG_MSG("triggers", line);

    if (line == "TRUE")
      runTriggers = true;
    else if (line == "FALSE")
      runTriggers = false;
    else
      throw janosh_exception() << string_info( { "Illegal triggers line", line });

    std::getline(response_stream, line);
    LOG_DEBUG_MSG("targets", line);

    splitAndPushBack(line, vecTargets);

    std::getline(response_stream, line);
    LOG_DEBUG_MSG("verbose", line);

    if (line == "TRUE")
      verbose = true;
    else if (line == "FALSE")
      verbose = false;
    else
      throw janosh_exception() << string_info( { "Illegal verbose line", line });

    LOG_INFO_STR(reconstructCommandLine(format, command, vecArgs, vecTargets, runTriggers, verbose));

    // only "-j get /." is cached
    bool cacheable = command == "get"
        && format == janosh::Json
        && !runTriggers
        && vecTargets.empty()
        && vecArgs.size() == 1
        && vecArgs[0] == "/.";

    if(!cacheable && (!command.empty() && command != "get" && command != "dump" && command != "hash")) {
      LOG_DEBUG_STR("Invalidating cache");
      cache_.invalidate();
    }

    bool cachehit = cacheable && cache_.isValid();

    LOG_DEBUG_MSG("cacheable", cacheable);
    LOG_DEBUG_MSG("cachehit", cachehit);

    if(!cachehit) {
      boost::asio::streambuf* out_buf = new boost::asio::streambuf();
      ostream* out_stream = new ostream(out_buf);

      JanoshThread* jt = new JanoshThread(format, command, vecArgs, vecTargets, runTriggers, verbose, *out_stream);

      int rc = jt->run();
      boost::asio::streambuf rc_buf;
      ostream rc_stream(&rc_buf);
      rc_stream << std::to_string(rc) << '\n';
      LOG_DEBUG_MSG("sending", rc_buf.size());
      boost::asio::write(*socket, rc_buf);

      std::thread flusher([=]{
        jt->join();
        LOG_DEBUG_MSG("sending", out_buf->size());
        if(cacheable) {
          LOG_DEBUG_STR("updating cache");
          cache_.setData(boost::asio::buffer_cast<const char*>(out_buf->data()), out_buf->size());
        }
        boost::asio::write(*socket, *out_buf);
        socket->close();
        delete out_buf;
        delete out_stream;
        delete jt;
        delete socket;
      });

      flusher.detach();
    } else {
       boost::asio::streambuf rc_buf;
       ostream rc_stream(&rc_buf);
       rc_stream << std::to_string(0) << '\n';
       LOG_DEBUG_MSG("sending", rc_buf.size());
       boost::asio::write(*socket, rc_buf);
       LOG_DEBUG_MSG("sending", cache_.getSize());
       boost::asio::write(*socket, boost::asio::buffer(cache_.getData(), cache_.getSize()));
       socket->close();
       delete socket;
    }
  } catch (janosh_exception& ex) {
    printException(ex);
    socket->close();
    delete socket;
  } catch (std::exception& ex) {
    printException(ex);
    socket->close();
    delete socket;
  }
}
} /* namespace janosh */
