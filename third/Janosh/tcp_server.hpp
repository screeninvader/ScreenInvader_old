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

namespace janosh {
using boost::asio::ip::tcp;
class TcpServer {
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  static TcpServer* instance_;
  TcpServer();

public:
	virtual ~TcpServer();
	bool isOpen();
  void open(int port);
	void close();
	bool run();

	static TcpServer* getInstance() {
	  if(instance_ == NULL)
	    instance_ = new TcpServer();
	  return instance_;
	}
};

} /* namespace janosh */

#endif /* CHEESYSERVER_H_ */
