
#include "util.hpp"
#include "exception.hpp"

namespace janosh {

string read_proc_val(const string& filename) {
  string v;
  ifstream in(filename, std::ios::in | std::ios::binary);

  try {
    if (in) {
      getline(in, v);
      in.close();
      return v;
    }
  } catch(std::exception& ex) {
  }

  throw janosh_exception() << string_info({"Unable to get value from", filename});
  return v;
}

ProcessInfo get_process_info(__pid_t pid) {
  string procDir = "/proc/" + std::to_string(pid) + "/";
  ProcessInfo pinfo;
  pinfo.cmdline_ = read_proc_val(procDir + "cmdline");
  pinfo.pid_ = pid;
  return pinfo;
}

ProcessInfo get_parent_info() {
  return get_process_info(getppid());
}

} /* namespace janosh */
