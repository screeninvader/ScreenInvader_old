#include <iostream>
#include <ucontext.h>
#include "exception.hpp"

#ifdef _JANOSH_DEBUG
#include "backward.h"
#endif

namespace janosh {
janosh_exception::janosh_exception() {
#ifdef _JANOSH_DEBUG
  using namespace backward;
  StackTrace st;
  void* error_addr = 0;

#if defined(REG_R14)
  register void* r14 asm ("r14");
  error_addr = r14;
#else
# warning ":/ sorry, ain't know no nothing none not of your architecture!"
#endif
  if (error_addr) {
    st.load_from(error_addr, 32);
  } else {
    st.load_here(32);
  }

  Printer printer;
  printer.address = true;
  printer.print(st, stderr);
#endif
}

void printException(std::exception& ex) {
  std::cerr << "Exception: " << ex.what() << std::endl;
}

void printException(janosh::janosh_exception& ex) {
  using namespace boost;

  std::cerr << "Exception: " << std::endl;

  if (auto const* m = get_error_info<msg_info>(ex))
    std::cerr << "message: " << *m << std::endl;

  if (auto const* vs = get_error_info<string_info>(ex)) {
    for (auto it = vs->begin(); it != vs->end(); ++it) {
      std::cerr << "info: " << *it << std::endl;
    }
  }

  if (auto const* ri = get_error_info<record_info>(ex))
    std::cerr << ri->first << ": " << ri->second << std::endl;

  if (auto const* pi = get_error_info<path_info>(ex))
    std::cerr << pi->first << ": " << pi->second << std::endl;

  if (auto const* vi = get_error_info<value_info>(ex))
    std::cerr << vi->first << ": " << vi->second << std::endl;
}
}

