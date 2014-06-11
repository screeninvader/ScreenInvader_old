#ifndef EXITHANDLER_HPP_
#define EXITHANDLER_HPP_

#include <stddef.h>
#include <vector>
#include <functional>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

namespace janosh {
using std::function;
using std::vector;

class ExitHandler {
public:
  typedef std::function<void()> ExitFunc;
  static ExitHandler* getInstance();
  void addExitFunc(ExitFunc e);
private:
  boost::asio::io_service ioservice_;
  boost::asio::signal_set signals_;
  static ExitHandler* instance_;
  vector<ExitFunc> exitFuncs_;
  ExitHandler();
  void runExitFuncs(const boost::system::error_code& error, int signal_number);
};

} /* namespace janosh */

#endif /* EXITHANDLER_HPP_ */
