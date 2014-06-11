/*
 * janosh_thread.cpp
 *
 *  Created on: Feb 2, 2014
 *      Author: elchaschab
 */

#include "tcp_worker.hpp"
#include "exception.hpp"
#include "database_thread.hpp"
#include "flusher_thread.hpp"
#include "cache_thread.hpp"
#include "trigger_thread.hpp"
#include "janosh.hpp"
#include "cache.hpp"

namespace janosh {

TcpWorker::TcpWorker(socket_ptr socket) :
    JanoshThread("TcpWorker"),
    socket_(socket) {
}

void shutdown(socket_ptr s) {
  if (s != NULL) {
    LOG_DEBUG_MSG("Closing socket", s);
    s->shutdown(boost::asio::socket_base::shutdown_both);
    s->close();
  }
}

void writeReturnCode(socket_ptr socket, int rc) {
  boost::asio::streambuf rc_buf;
  ostream rc_stream(&rc_buf);
  rc_stream << std::to_string(rc) << '\n';
  LOG_DEBUG_MSG("sending", rc_buf.size());
  boost::asio::write(*socket, rc_buf);
}

string reconstructCommandLine(Request& req) {
  string cmdline = "janosh ";

  if(req.verbose_)
    cmdline += "-v ";

  if(req.format_ == janosh::Bash)
    cmdline += "-b ";
  else if(req.format_ == janosh::Json)
    cmdline += "-j ";
  else if(req.format_ == janosh::Raw)
    cmdline += "-r ";

  if(req.runTriggers_)
     cmdline += "-t ";

   if(!req.vecTargets_.empty()) {
     cmdline += "-e ";
     bool first = true;
     for(const string& target : req.vecTargets_) {
       if(!first)
         cmdline+=",";
       cmdline+=target;

       first = false;
     }
     cmdline += " ";
   }

   cmdline += (req.command_ + " ");

   if(!req.vecArgs_.empty()) {
     bool first = true;
     for(const string& arg : req.vecArgs_) {
       if(!first)
         cmdline+=" ";
       cmdline+= ("\"" + arg + "\"");

       first = false;
     }
     cmdline += " ";
   }

  return cmdline;
}

void TcpWorker::run() {
  try {
    Request req;
    bool cacheable = false;
    bool cachehit = false;
    std::string peerAddr = socket_->remote_endpoint().address().to_string();
    LOG_DEBUG_MSG("accepted", peerAddr);
    LOG_DEBUG_MSG("socket", socket_);

    boost::asio::streambuf response;
    boost::asio::read_until(*socket_, response, "\n");
    std::istream response_stream(&response);

    read_request(req, response_stream);

    LOG_DEBUG_MSG("ppid", req.pinfo_.pid_);
    LOG_DEBUG_MSG("cmdline", req.pinfo_.cmdline_);
    LOG_INFO_STR(reconstructCommandLine(req));

    // only "-j get /." is cached
    cacheable = req.command_ == "get" && req.format_ == janosh::Json && !req.runTriggers_ && req.vecTargets_.empty() && req.vecArgs_.size() == 1
        && req.vecArgs_[0] == "/.";

    Cache* cache = Cache::getInstance();
    if (!cacheable && (!req.command_.empty() && (req.command_ != "get" && req.command_ != "dump" && req.command_ != "hash" && req.command_ != "size"))) {
      LOG_DEBUG_STR("Invalidating cache");
      cache->invalidate();
    }

    cachehit = cacheable && cache->isValid();

    LOG_DEBUG_MSG("cacheable", cacheable);
    LOG_DEBUG_MSG("cachehit", cachehit);

    Janosh* instance = Janosh::getInstance();
    instance->setFormat(req.format_);

    if (!cachehit) {
      streambuf_ptr out_buf(new boost::asio::streambuf());
      ostream_ptr out_stream(new ostream(out_buf.get()));

      if (!req.command_.empty()) {
        DatabaseThread* dt = new DatabaseThread(req, out_stream);
        dt->runSynchron();
        bool result = dt->result();

        // report return code
        writeReturnCode(socket_, result ? 0 : 1);

        if (!result) {
          shutdown(socket_);
          return;
        }
      } else {
        writeReturnCode(socket_, 0);
      }

      if (req.runTriggers_ || !req.vecTargets_.empty()) {
        TriggerThread* triggerThread = new TriggerThread(req, out_stream);
        triggerThread->runSynchron();
        if (!triggerThread->result()) {
          shutdown(socket_);
          return;
        }
      }

      FlusherThread* flusher = new FlusherThread(socket_, out_buf, cacheable);
      flusher->runSynchron();
      shutdown(socket_);
    } else {
      CacheThread* cacheWriter = new CacheThread(socket_);
      cacheWriter->runSynchron();
      shutdown(socket_);
    }
  } catch (std::exception& ex) {
    janosh::printException(ex);
    shutdown(socket_);
  }
}
} /* namespace janosh */
