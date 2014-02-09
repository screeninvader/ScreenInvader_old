/*
 * CheesyServer.h
 *
 *  Created on: Jan 6, 2014
 *      Author: elchaschab
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <boost/asio.hpp>
#include "format.hpp"
#include "cache.hpp"

namespace janosh {

class TcpServer {
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::acceptor acceptor;
  static TcpServer* instance_;
  Cache cache_;
  TcpServer();

public:
	virtual ~TcpServer();
	bool isOpen();
  void open(int port);
	void close();
	void run();

	static TcpServer* getInstance() {
	  if(instance_ == NULL)
	    instance_ = new TcpServer();
	  return instance_;
	}
};

} /* namespace janosh */

#endif /* CHEESYSERVER_H_ */
