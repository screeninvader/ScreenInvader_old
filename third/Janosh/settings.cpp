/*
 * settings.cpp
 *
 *  Created on: Feb 9, 2014
 *      Author: elchaschab
 */

#include "settings.hpp"
#include <fstream>
#include <exception>


namespace janosh {
using std::ifstream;
using std::exception;
Settings::Settings() {
   const char* home = getenv ("HOME");
   if (home==NULL) {
     error("Can't find environment variable.", "HOME");
   }
   string janoshDir = string(home) + "/.janosh/";
   fs::path dir(janoshDir);

   if (!fs::exists(dir)) {
     if (!fs::create_directory(dir)) {
       error("can't create directory", dir.string());
     }
   }

   this->janoshFile = fs::path(dir.string() + "janosh.json");
   this->triggerFile = fs::path(dir.string() + "triggers.json");
   this->logFile = fs::path(dir.string() + "janosh.log");

   if(!fs::exists(janoshFile)) {
     error("janosh configuration not found: ", janoshFile);
   } else {
     js::Value janoshConf;

     try{
       ifstream is(janoshFile.c_str());
       js::read(is,janoshConf);
       js::Object jObj = janoshConf.get_obj();
       js::Value v;

       if(find(jObj, "triggerDirectories", v)) {
         for (const js::Value& vDir: v.get_array()) {
           triggerDirs.push_back(fs::path(vDir.get_str() + "/"));
         }
       }

       if(find(jObj, "database", v)) {
         this->databaseFile = fs::path(v.get_str());
       } else {
         error(this->janoshFile.string(), "No database file defined");
       }

       if(find(jObj, "port", v)) {
         this->port = std::stoi(v.get_str());
       } else {
         error(this->janoshFile.string(), "No database file defined");
       }
     } catch (exception& e) {
       error("Unable to load janosh configuration", e.what());
     }
   }
 }

 bool Settings::find(const js::Object& obj, const string& name, js::Value& value) {
   auto it = find_if(obj.begin(), obj.end(),
       [&](const js::Pair& p){ return p.name_ == name;});
   if (it != obj.end()) {
     value = (*it).value_;
     return true;
   }
   return false;
 }
}
/* namespace janosh */
