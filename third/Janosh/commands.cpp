#include "commands.hpp"
#include "exception.hpp"

namespace janosh {

class RemoveCommand: public Command {
public:
  RemoveCommand(Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const std::vector<string>& params, std::ostream& out) {
    if (!params.empty()) {
      int cnt = 0;
      for(const string& p : params) {
        LOG_DEBUG_MSG("Removing", p);
//FIXME use cursors for batch operations
        Record path(p);
        cnt += janosh->remove(path);
        LOG_DEBUG(cnt);
      }

      if (cnt == 0)
        return {0, "No entries removed"};
      else
        return {cnt, "Successful"};
    } else {
      return {-1, "Too few parameters"};
    }
  }
};
class HashCommand: public Command {
public:
  HashCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (!params.empty()) {
      LOG_DEBUG_STR("hash doesn't take any parameters");
      return {-1, "Failed"};
    } else {
      return {janosh->hash(), "Successful"};
    }
  }
};

class LoadCommand: public Command {
public:
  LoadCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.empty()) {
      janosh->loadJson(std::cin);
    } else {
      for(const string& p : params) {
        janosh->loadJson(p);
      }
    }
    return {0, "Successful"};
  }
};

class MakeArrayCommand: public Command {
public:
  MakeArrayCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() != 1)
      return {0, "Expected one path"};

    if (janosh->makeArray(Record(params.front())))
      return {1, "Successful"};
    else
      return {-1, "Failed"};
  }
};

class MakeObjectCommand: public Command {
public:
  MakeObjectCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() != 1)
      return {0, "Expected one path"};

    if (janosh->makeObject(Record(params.front())))
      return {1, "Successful"};
    else
      return {-1, "Failed"};
  }
};

class TruncateCommand: public Command {
public:
  TruncateCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (!params.empty())
      return {0, "Truncate doesn't take any arguments"};

    return {janosh->truncate(), "Successful"};
  }
};

class AddCommand: public Command {
public:
  AddCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() % 2 != 0) {
      return {-1, "Expected a list of path/value pairs"};
    } else {
      const string path = params.front();

      for (auto it = params.begin(); it != params.end(); it += 2) {
        if (!janosh->add(Record(*it), *(it + 1)))
          return {-1, "Failed"};
      }

      return {params.size()/2, "Successful"};
    }
  }
};

class ReplaceCommand: public Command {
public:
  ReplaceCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.empty() || params.size() % 2 != 0) {
      return {-1, "Expected a list of path/value pairs"};
    } else {

      for (auto it = params.begin(); it != params.end(); it += 2) {
        if (!janosh->replace(Record(*it), *(it + 1)))
          return {-1, "Failed"};
      }

      return {params.size()/2, "Successful"};
    }
  }
};

class SetCommand: public Command {
public:
  SetCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.empty() || params.size() % 2 != 0) {
      return {-1, "Expected a list of path/value pairs"};
    } else {

      size_t cnt = 0;
      for (auto it = params.begin(); it != params.end(); it += 2) {
        cnt += janosh->set(Record(*it), *(it + 1));
      }

      if (cnt == 0)
        return {-1, "Failed"};
      else
        return {cnt, "Successful"};
    }
  }
};

class ShiftCommand: public Command {
public:
  ShiftCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() != 2) {
      return {-1, "Expected two paths"};
    } else {
      Record src(params.front());
      Record dest(params.back());
      src.fetch();

      if (!src.exists())
        return {-1, "Source path doesn't exist"};

      size_t cnt = janosh->shift(src, dest);
      if (cnt == 0)
        return {-1, "Failed"};
      else
        return {1, "Successful"};
    }
  }
};

class CopyCommand: public Command {
public:
  CopyCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() != 2) {
      return {-1, "Expected two paths"};
    } else {
      Record src(params.front());
      Record dest(params.back());
      src.fetch();

      if (!src.exists())
        return {0, "Source path doesn't exist"};

      size_t n = janosh->copy(src, dest);
      if (n > 0)
        return {n, "Successful"};
      else
        return {-1, "Failed"};
    }
  }
};

class MoveCommand: public Command {
public:
  MoveCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() != 2) {
      return {-1, "Expected two paths"};
    } else {
      Record src(params.front());
      Record dest(params.back());
      src.fetch();

      if (!src.exists())
        return {0, "Source path doesn't exist"};

      size_t n = janosh->move(src, dest);
      if (n > 0)
        return {n, "Successful"};
      else
        return {-1, "Failed"};
    }
  }
};

class AppendCommand: public Command {
public:
  AppendCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() < 2) {
      return {-1, "Expected a path and a list of values"};
    } else {
      Record target(params.front());
      size_t cnt = janosh->append(params.begin() + 1, params.end(), target);
      return {cnt, "Successful"};
    }
  }
};

class DumpCommand: public Command {
public:
  DumpCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (!params.empty()) {
      return {-1, "Dump doesn't take any parameters"};
    } else {
      janosh->dump(out);
    }
    return {0, "Successful"};
  }
};

class SizeCommand: public Command {
public:
  SizeCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.size() != 1) {
      return {-1, "Expected a path"};
    } else {
      Record p(params.front());
      out << janosh->size(p) << std::endl;
    }
    return {0, "Successful"};
  }
};

class TriggerCommand: public Command {
public:
  TriggerCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.empty()) {
      return {-1, "Expected a list of triggers"};
    } else {
      for(const string& p : params) {
        LOG_INFO_MSG("Execute trigger", p);
        try {
        janosh->triggers_.executeTrigger(Path(p), out);
        } catch (path_exception&  ex) {
          janosh::printException(ex);
        }
      }

      return {params.size(), "Successful"};
    }
  }
};

class GetCommand: public Command {
public:
  GetCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  Result operator()(const vector<string>& params, std::ostream& out) {
    if (params.empty()) {
      return {-1, "Expected a list of keys"};
    } else {
      std::vector<Record> recs;
      for(const string& p : params) {
        recs.push_back(Record(p));
      }

      if (!janosh->get(recs, out))
        return {-1, "Fetching failed"};
    }
    return {params.size(), "Successful"};
  }
};

class TargetCommand: public Command {
public:
  TargetCommand(janosh::Janosh* janosh) :
      Command(janosh) {
  }

  virtual Result operator()(const vector<string>& params, std::ostream& out) {
    size_t cnt = 0;
    if (params.empty())
      return {-1, "Expected a list of targets"};

    for(const string& t : params) {
      janosh->triggers_.executeTarget(t,out);
      ++cnt;
    }

    return {cnt, "Successful"};
  }
};

CommandMap makeCommandMap(Janosh* janosh) {
  CommandMap cm;
  cm.insert( { "load", new LoadCommand(janosh) });
  cm.insert( { "set", new SetCommand(janosh) });
  cm.insert( { "add", new AddCommand(janosh) });
  cm.insert( { "replace", new ReplaceCommand(janosh) });
  cm.insert( { "append", new AppendCommand(janosh) });
  cm.insert( { "dump", new DumpCommand(janosh) });
  cm.insert( { "size", new SizeCommand(janosh) });
  cm.insert( { "get", new GetCommand(janosh) });
  cm.insert( { "copy", new CopyCommand(janosh) });
  cm.insert( { "remove", new RemoveCommand(janosh) });
  cm.insert( { "shift", new ShiftCommand(janosh) });
  cm.insert( { "move", new MoveCommand(janosh) });
  cm.insert( { "trigger", new TriggerCommand(janosh) });
  cm.insert( { "target", new TargetCommand(janosh) });
  cm.insert( { "truncate", new TruncateCommand(janosh) });
  cm.insert( { "mkarr", new MakeArrayCommand(janosh) });
  cm.insert( { "mkobj", new MakeObjectCommand(janosh) });
  cm.insert( { "hash", new HashCommand(janosh) });
  return cm;
}
}
