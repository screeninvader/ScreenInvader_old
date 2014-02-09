
#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "format.hpp"

namespace janosh {
using std::string;
using std::vector;

class TcpClient {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket;
public:
	TcpClient();
	virtual ~TcpClient();
	void connect(std::string host, int port);
	int run(Format f, string command, vector<string> args, vector<string> vecTargets, bool triggers, bool verbose);
	void close();
};

} /* namespace janosh */

#endif /* CHEESYCLIENT_H_ */
