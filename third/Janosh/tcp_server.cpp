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
#include <boost/bind.hpp>

#include "tcp_server.hpp"
#include "format.hpp"
#include "logger.hpp"
#include "tcp_worker.hpp"

#include "exception.hpp"
#include "exithandler.hpp"
#include "shared_pointers.hpp"

namespace janosh {

using boost::asio::ip::tcp;
using std::string;
using std::vector;
using std::stringstream;
using std::function;
using std::ostream;

TcpServer* TcpServer::instance_;

TcpServer::TcpServer() : io_service_(), acceptor_(io_service_){
  ExitHandler::getInstance()->addExitFunc([&](){this->close();});
}

void TcpServer::open(int port) {
  tcp::resolver resolver(io_service_);
  tcp::resolver::query query("0.0.0.0", std::to_string(port));
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
}

TcpServer::~TcpServer() {
  this->close();
}

bool TcpServer::isOpen() {
  return acceptor_.is_open();
}

void TcpServer::close() {
  io_service_.stop();
}

bool TcpServer::run() {
  socket_ptr socket = NULL;

	try  {
	  socket = socket_ptr(new tcp::socket(io_service_));
	  acceptor_.accept(*socket);
	} catch(std::exception& ex) {
	  if (socket != NULL) {
	    LOG_DEBUG_MSG("Closing socket", socket);
	    socket->shutdown(boost::asio::socket_base::shutdown_both);
	    socket->close();
	  }
	  janosh::printException(ex);
	  return false;
	}

	try {
	  TcpWorker* w = new TcpWorker(socket);
	  w->runAsynchron();
  } catch (janosh_exception& ex) {
    if (socket != NULL) {
      LOG_DEBUG_MSG("Closing socket", socket);
      socket->shutdown(boost::asio::socket_base::shutdown_both);
      socket->close();
    }
    printException(ex);
  } catch (std::exception& ex) {
    if (socket != NULL) {
      LOG_DEBUG_MSG("Closing socket", socket);
      socket->shutdown(boost::asio::socket_base::shutdown_both);
      socket->close();
    }
    printException(ex);
  }
  return true;
}
} /* namespace janosh */
