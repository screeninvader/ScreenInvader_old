#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include "janosh.hpp"
#include <boost/range.hpp>
#include <boost/tokenizer.hpp>
#include <string>
#include <iostream>

namespace janosh {
  using std::string;
  using boost::tokenizer;
  using boost::char_separator;

  class Command {
  protected:
    janosh::Janosh * janosh;
  public:
    typedef std::pair<int32_t, string> Result;

    Command(janosh::Janosh* janosh) :
        janosh(janosh) {
    }

    virtual ~Command() {
    }

    virtual Result operator()(const vector<string>& params) {
      return {-1, "Not implemented" };
    };
  };

  class RemoveCommand: public Command {
  public:
    RemoveCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (!params.empty()) {
        int cnt = 0;
        BOOST_FOREACH(const string& p, params) {
          LOG_DEBUG_MSG("Removing", p);
//FIXME use cursors for batch operations
          DBPath path(p);
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

    virtual Result operator()(const vector<string>& params) {
      if (!params.empty()) {
        LOG_DEBUG_STR("hash doesn't take any parameters");
        return {0, "Failed"};
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

    virtual Result operator()(const vector<string>& params) {
      if (params.empty()) {
        janosh->loadJson(std::cin);
      } else {
        BOOST_FOREACH(const string& p, params) {
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

    virtual Result operator()(const vector<string>& params) {
      if (params.size() != 1)
        return {0, "Expected one path" };

      if(janosh->makeArray(params.front()))
        return {1, "Successful"};
      else
        return {0, "Failed"};
    }
  };

  class MakeObjectCommand: public Command {
  public:
    MakeObjectCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (params.size() != 1)
        return {0, "Expected one path" };

      if(janosh->makeObject(params.front()))
        return {1, "Successful"};
      else
        return {0, "Failed"};
    }
  };

  class TruncateCommand: public Command {
  public:
    TruncateCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (!params.empty())
        return {0, "Truncate doesn't take any arguments" };

      return {janosh->truncate(), "Successful"};
    }
  };

  class AddCommand: public Command {
  public:
    AddCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (params.size() % 2 != 0) {
        return {-1, "Expected a list of path/value pairs"};
      } else {
        const string path = params.front();

        for (auto it = params.begin(); it != params.end(); it+=2) {
          if(!janosh->add(*it, *(it+1)))
            return {0, "Failed"};
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

    virtual Result operator()(const vector<string>& params) {
      if (params.empty() || params.size() % 2 != 0) {
        return {-1, "Expected a list of path/value pairs"};
      } else {
        for (auto it = params.begin(); it != params.end(); it += 2) {
          if(!janosh->replace(DBPath(*it), *(it + 1)))
            return {0, "Failed"};
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

    virtual Result operator()(const vector<string>& params) {
      if (params.empty() || params.size() % 2 != 0) {
        return {-1, "Expected a list of path/value pairs"};
      } else {
        for (auto it = params.begin(); it != params.end(); it += 2) {
          janosh->set(DBPath(*it), *(it + 1));
        }

        return {params.size()/2, "Successful"};
      }
    }
  };

  class ShiftCommand: public Command {
  public:
    ShiftCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (params.size() != 2) {
        return {-1, "Expected two paths"};
      } else {
        DBPath src(params.front());
        DBPath dest(params.back());
        src.prune();

        if(!src.exists())
          return {-1, "Source path doesn't exist"};

        janosh->shift(src, dest);
        return {1, "Successful"};
      }
    }
  };

  class CopyCommand: public Command {
  public:
    CopyCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (params.size() != 2) {
        return {-1, "Expected two paths"};
      } else {
        DBPath src(params.front());
        DBPath dest(params.back());
        src.prune();

        if(!src.exists())
          return {-1, "Source path doesn't exist"};

        janosh->copy(src, dest);
        return {1, "Successful"};
      }
    }
  };

  class AppendCommand: public Command {
  public:
    AppendCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (params.size() < 2) {
        return {-1, "Expected a path and a list of values"};
      } else {
        DBPath target(params.front());
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

    virtual Result operator()(const vector<string>& params) {
      if (!params.empty()) {
        return {-1, "Dump doesn't take any parameters"};
      } else {
        janosh->dump();
      }
      return {0, "Successful"};
    }
  };

  class SizeCommand: public Command {
  public:
    SizeCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (params.size() != 1) {
        return {-1, "Expected a path"};
      } else {
        DBPath p(params.front());
        std::cout << janosh->size(p) << std::endl;
      }
      return {0, "Successful"};
    }
  };

  class TriggerCommand: public Command {
  public:
    TriggerCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      if (params.empty()) {
        return {-1, "Expected a list of triggers"};
      } else {
        BOOST_FOREACH(const string& p, params) {
          janosh->triggers.executeTrigger(p);
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

    Result operator()(const vector<string>& params) {
      if (params.empty()) {
        return {-1, "Expected a list of keys"};
      } else {
        bool found_all = true;
        BOOST_FOREACH(const string& p, params) {
          DBPath path(p);
          found_all = found_all && janosh->print(path, std::cout);
        }

        if (!found_all)
          return {-1, "Unknown keys encounted"};
      }
      return {params.size(), "Successful"};
    }
  };

  class TargetCommand: public Command {
  public:
    TargetCommand(janosh::Janosh* janosh) :
        Command(janosh) {
    }

    virtual Result operator()(const vector<string>& params) {
      size_t cnt = 0;
      if(params.empty())
        return {-1, "Expected a list of targets"};

      BOOST_FOREACH (const string& t, params) {
        janosh->triggers.executeTarget(t);
        ++cnt;
      }

      return {cnt, "Successful"};
    }
  };
}
#endif
