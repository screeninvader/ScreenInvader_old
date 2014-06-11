#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <string>
#include <exception>

#include <boost/exception/all.hpp>
#include "record.hpp"
#include "path.hpp"
#include "value.hpp"

namespace janosh {
  typedef boost::error_info<struct tag_janosh_msg,std::string> msg_info;
  typedef boost::error_info<struct tag_janosh_string,std::vector<string> > string_info;
  typedef boost::error_info<struct tag_janosh_record,std::pair<string,Record> > record_info;
  typedef boost::error_info<struct tag_janosh_path,std::pair<string,Path> > path_info;
  typedef boost::error_info<struct tag_janosh_path,std::pair<std::string,std::string> > value_info;

  struct janosh_exception :
    virtual boost::exception,
    virtual std::exception
  {
      janosh_exception();
  };

  struct db_exception : virtual janosh_exception
  {};
  struct config_exception : virtual janosh_exception
  {};

  struct record_exception : virtual janosh_exception
  {};

  struct path_exception : virtual janosh_exception
  {};

  struct value_exception : virtual janosh_exception
  {};

  void printException(std::exception& ex);
  void printException(janosh::janosh_exception& ex);
}

#endif /* EXCEPTION_HPP_ */
