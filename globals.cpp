#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <cstring>
#include <ctime>
#include <cstdlib>

#include "debug.hpp"
#include "globals.hpp"

namespace {
  void terminator();
  void mkProjDir(const char *);
}

namespace Globals {
/**********************************************************
 * preliminary initialization of global variables  
 **********************************************************/
  std::string progName; 
  std::string progDir;

/**********************************************************
 * functions' definitions
 **********************************************************/
  void On() {
    static bool flag = false;
    if (flag) {
      CRASH << __PRETTY_FUNCTION__ << " was called twice" << std::endl;
    }
    flag = true;

    std::set_terminate(terminator);

    if (progName.size() == 0) {
      CRASH << "progName is unset" << std::endl;
    }
    
    std::string progNameCopy = progName;
    char * cName = strdup(progName.c_str());
    progDir = dirname(cName);
    free(cName);
    mkProjDir("logs");
    mkProjDir("cores");
    Debug::ErrorLogger::open("error.log");
    Debug::BasicLogger::open("basic.log");
    char timeBuf[32];
    time_t curTime = time(nullptr);
    ctime_r(&curTime, timeBuf);
    BasicLog << "Program started at " << timeBuf << std::flush;
  }

  void Off() {
    static bool flag = false;
    if (flag) {
      CRASH << __PRETTY_FUNCTION__ << " was called twice" << std::endl;
    }
    flag = true;

    char timeBuf[32];
    time_t curTime = time(nullptr);
    ctime_r(&curTime, timeBuf);
    BasicLog << "Program ended at " << timeBuf << std::endl;
    Debug::ErrorLogger::close();
    Debug::BasicLogger::close();
  }
} /* namespace Globals */

/**********************************************************
 * anonymous namespace
 **********************************************************/
namespace {
  // function to be called instead of default terminate()
  void terminator() {
    char timeBuf[32];
    time_t curTime = time(nullptr);
    ctime_r(&curTime, timeBuf);
    ErrorLog << "CRASH: terminate() at " 
		      << timeBuf << std::endl;
    Globals::Off();
    abort();
  }

  // make subdirectory `dirName' in project's directory
  void mkProjDir(const char * dirName) {
    using Globals::progDir;
    using std::endl;
    std::string projDir = progDir + '/' + dirName;
    int rCode, eCode; // return and error codes
    const int errMsgSize = 256;
    char errMsg[errMsgSize];
    struct stat projDirStat;
    if ((rCode = stat(projDir.c_str(), &projDirStat)) != 0) {
      eCode = errno;
      if ( eCode == ENOENT ) {
	rCode = mkdir(projDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	if (rCode != 0) {
	  eCode = errno;
	  strerror_r(eCode, errMsg, errMsgSize);
	  CRASH << "Failed creating " << projDir << "/: " << errMsg << endl;
	}
      } else {
	strerror_r(eCode, errMsg, errMsgSize);
	CRASH << "Failed stating " << projDir << "/: " << errMsg << endl;
      }
    } else {
      if (!S_ISDIR(projDirStat.st_mode)) {
	CRASH << projDir << "/ is not a directory" << endl;
      }
    }
  }
} /* anonymous namespace */
