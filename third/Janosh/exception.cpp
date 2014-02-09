#include "exception.hpp"

namespace janosh {
  void printException(std::exception& ex) {
    std::cerr << "Exception: " << ex.what() << std::endl;
  }

  void printException(janosh::janosh_exception& ex) {
    using namespace boost;

    std::cerr << "Exception: " << std::endl;
  #if 0
    std::cerr << boost::trace(ex) << std::endl;
  #endif
    if(auto const* m = get_error_info<msg_info>(ex) )
      std::cerr << "message: " << *m << std::endl;

    if(auto const* vs = get_error_info<string_info>(ex) ) {
      for(auto it=vs->begin(); it != vs->end(); ++it) {
        std::cerr << "info: " << *it << std::endl;
      }
    }

    if(auto const* ri = get_error_info<record_info>(ex))
      std::cerr << ri->first << ": " << ri->second << std::endl;

    if(auto const* pi = get_error_info<path_info>(ex))
      std::cerr << pi->first << ": " << pi->second << std::endl;

    if(auto const* vi = get_error_info<value_info>(ex))
      std::cerr << vi->first << ": " << vi->second << std::endl;
  }
}

