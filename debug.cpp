#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "debug.hpp"
#include "globals.hpp"

namespace Debug {
  BasicLogger BasicLogger::instance;

  BasicLogger::~BasicLogger() {
    if (logFile.is_open()) {
      ErrorLog << "Destructor was called for open BasicLogger" << std::endl;
      logFile.close();
    }
  }

  void BasicLogger::open(const char * name) {
    using Globals::progDir;
    if (instance.logFile.is_open()) {
      CRASH << "Reinitialization of the open logger" << std::endl;
    }
    if (progDir.size() == 0) {
      CRASH << "progDir is not set" << std::endl;
    }
    std::string fileName = progDir + "/logs/" + name;
    instance.logFile.open(fileName.c_str(), std::ios_base::out | std::ios_base::app);
    if (!instance.logFile.is_open()) {
      CRASH << "Failed opening " << fileName << " for logging" << std::endl;
    }
  }
    
  BasicLogger & BasicLogger::operator<<(std::ostream & (*manip)(std::ostream &)) {
    logFile << manip;
    return *this;
  }

  ErrorLogger ErrorLogger::instance;

  ErrorLogger::~ErrorLogger() {
    if (logFile.is_open()) {
      ErrorLog << "Destructor was called for open BasicLogger" << std::endl;
      logFile.close();
    }
  }

  void ErrorLogger::open(const char * name) {
    using Globals::progDir;
    if (instance.logFile.is_open()) {
      CRASH << "Reinitialization of the open logger" << std::endl;
    }
    if (progDir.size() == 0) {
      CRASH << "progDir is not set" << std::endl;
    }
    std::string fileName = progDir + "/logs/" + name;
    instance.logFile.open(fileName.c_str(), std::ios_base::out | std::ios_base::app);
    if (!instance.logFile.is_open()) {
      CRASH << "Failed opening " << fileName << " for logging" << std::endl;
    }
  }
  
  ErrorLogger & ErrorLogger::operator<<(std::ostream & (*manip)(std::ostream &)) {
    BasicLog << manip;
    std::clog << manip;
    logFile << manip;
    return *this;
  }

  Exception & Exception::operator<<(std::ostream & (*manip)(std::ostream &)) {
    ErrorLog << manip;
    msg << manip;
    return *this;
  }

  Exception::Exception(const char * a, const char * b, const char * c) {
    msg << a << ':' << b << ':' << c << ':';
    ErrorLog << msg.str();
  }

  void printStack() {
    using std::string;
    using std::ofstream;
    using std::setw;
    using std::setfill;
    using std::endl;
    using Globals::progName;
    using Globals::progDir;

    if (progName.size() == 0 or progDir.size()  == 0) {
      throw Crash(AT) << "Either progName or progDir is unset" << endl; 
    }

    tm ct; // current time
    time_t epochTime = time(nullptr);
    localtime_r(&epochTime, &ct);

    std::ostringstream coreName;
    coreName << progDir
	     << "/cores/core." << setfill('0')
	     << ct.tm_year + 1900 << '.' << setw(2)
	     << ct.tm_mon + 1 << '.' << setw(2)
	     << ct.tm_mday << '.' << setw(2)
	     << ct.tm_hour << '.' << setw(2)
	     << ct.tm_min << '.' << setw(2)
	     << ct.tm_sec;
    struct stat coreNameStat;
    int rCode, eCode;
    const int errMsgSize = 256;
    char errMsg[errMsgSize];
    rCode = stat(coreName.str().c_str(), &coreNameStat);
    eCode = errno;
    if (rCode == 0) {
      ErrorLog << coreName << " already exists, old file will be lost" << endl;
    } else if (eCode != ENOENT) { 
      strerror_r(eCode, errMsg, errMsgSize);
      ErrorLog << "Failed stating " << coreName << ": " << errMsg 
	       << " (Nonetheless I'll try to generate core dump)" << endl;
    }

    string gdbCommandsName = progDir + "/cores/" + "batch"; 

    ofstream gdbCommands; 
    gdbCommands.open(gdbCommandsName.c_str());
    if (!gdbCommands.is_open()) {
      throw Crash(AT) << "Failed opening " 
		      << gdbCommandsName << " file" << endl;
    }
  
    gdbCommands /*<< "set print address on\n"
		  << "show print address on\n"
		  << "set print frame-arguments all\n"*/
      << "backtrace"/* full*/"\n"
      << "gcore " << coreName.str() << '\n'
      << "detach\n"
      << "quit" << endl;
    gdbCommands.close(); // TODO check whether file output succeeded

    pid_t pid = getpid();
    std::ostringstream gdbCall;
    gdbCall << "gdb -q -batch " << progName << ' ' 
	    << pid << " -x " << gdbCommandsName << endl;
    TraceLog << gdbCall.str() << endl;
    system(gdbCall.str().c_str());
  }

} /* namespace Debug */
