/*
 * CheesyClient.cpp
 *
 *  Created on: Jan 6, 2014
 *      Author: elchaschab
 */

#include <iostream>
#include "tcp_client.hpp"
#include "logger.hpp"

namespace janosh {

using boost::asio::ip::tcp;

TcpClient::TcpClient() : socket(io_service) {
}

TcpClient::~TcpClient() {
  this->close();
}

void TcpClient::connect(string host, int port) {
	    tcp::resolver resolver(io_service);
	    tcp::resolver::query query(host, std::to_string(port));
	    tcp::resolver::iterator iterator = resolver.resolve(query);
	    boost::asio::connect(socket, iterator);
}

int TcpClient::run(Request& req) {
  boost::asio::streambuf request;
  std::ostream request_stream(&request);

  writeRequest(req, request_stream);

  boost::asio::write(socket, request);
	boost::asio::streambuf response;
	std::istream response_stream(&response);
  boost::asio::read_until(socket, response,"\n");

	string strReturnCode;
	std::getline(response_stream, strReturnCode);
  LOG_DEBUG_MSG("return code", strReturnCode);
	int returnCode = std::stoi(strReturnCode);

	try {
	  string line;
	  while(response_stream) {
	    boost::asio::read_until(socket, response,"\n");
	    std::getline(response_stream, line);
	    std::cout << line << std::endl;
	  }
	} catch(std::exception& ex) {
	}
	return returnCode;
}

void TcpClient::close() {
	socket.close();
}

} /* namespace janosh */
