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

string reconstructCommandLine(Request& req) {
  string cmdline = "janosh ";

  if(req.verbose_)
    cmdline += "-v ";

  if(req.format_ == janosh::Bash)
    cmdline += "-b ";
  else if(req.format_ == janosh::Json)
    cmdline += "-j ";
  else if(req.format_ == janosh::Raw)
    cmdline += "-r ";

  if(req.runTriggers_)
     cmdline += "-t ";

   if(!req.vecTargets_.empty()) {
     cmdline += "-e ";
     bool first = true;
     for(const string& target : req.vecTargets_) {
       if(!first)
         cmdline+=",";
       cmdline+=target;

       first = false;
     }
     cmdline += " ";
   }

   cmdline += (req.command_ + " ");

   if(!req.vecArgs_.empty()) {
     bool first = true;
     for(const string& arg : req.vecArgs_) {
       if(!first)
         cmdline+=" ";
       cmdline+= ("\"" + arg + "\"");

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
    std::string peerAddr = socket->remote_endpoint().address().to_string();

    LOG_DEBUG_MSG("accepted", peerAddr);
    boost::asio::streambuf response;
    boost::asio::read_until(*socket, response, "\n");
    std::istream response_stream(&response);

    Request req;
    readRequest(req, response_stream);

    LOG_INFO_STR(reconstructCommandLine(req));

    // only "-j get /." is cached
    bool cacheable = req.command_ == "get"
        && req.format_ == janosh::Json
        && !req.runTriggers_
        && req.vecTargets_.empty()
        && req.vecArgs_.size() == 1
        && req.vecArgs_[0] == "/.";

    if(!cacheable && (!req.command_.empty() && (req.command_ != "get" && req.command_ != "dump" && req.command_ != "hash"))) {
      LOG_DEBUG_STR("Invalidating cache");
      cache_.invalidate();
    }

    bool cachehit = cacheable && cache_.isValid();

    LOG_DEBUG_MSG("cacheable", cacheable);
    LOG_DEBUG_MSG("cachehit", cachehit);

    if(!cachehit) {
      boost::asio::streambuf* out_buf = new boost::asio::streambuf();
      ostream* out_stream = new ostream(out_buf);

      JanoshThread* jt = new JanoshThread(req, *out_stream);

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
