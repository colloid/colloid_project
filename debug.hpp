#ifndef DEBUG_HPP_
#define DEBUG_HPP_
#include <sstream>
#include <fstream>
#include <exception>
#include <iostream>

#define AT __FILE__, __TO_STR__( __LINE__ ), __PRETTY_FUNCTION__
#define __TO_STR__(x) __STRINGIFY__(x)
#define __STRINGIFY__(x) #x
#define CRASH Debug::printStack(); throw Debug::Crash(AT)

#define ErrorLog Debug::ErrorLogger::getInstance()
#define BasicLog Debug::BasicLogger::getInstance()

#define __TRACE__
#ifdef __TRACE__
#define TraceLog std::clog << "TRACE: " << __FILE__ << ':' << __LINE__ << ": "
#else
namespace Debug {
  struct phonyLog {
    template <typename T> phonyLog & operator<<(const T & t) { return *this; }
  };
}
#define TraceLog Debug::phonyLog()
#endif

namespace Debug {
  //! Печать стека при помощи внешнего отладчика
  void printStack(void);

/***********************************************************
 * Loggers
 * There's no base class for them because it's impossible
 * to make template methods virtual
 ***********************************************************/
  class BasicLogger { // singleton
    static BasicLogger instance;
    std::fstream logFile;
    BasicLogger() {}
    BasicLogger(const BasicLogger &) {};
  public:
    ~BasicLogger();
    static BasicLogger & getInstance() { return instance; }
    static void open(const char * name);
    static void close() { instance.logFile.close(); }
    template <typename T> 
    BasicLogger & operator<<(const T & t) {
      logFile << t;
      return *this;
    }
    BasicLogger & operator<<(std::ostream & (*manip)(std::ostream &));
  };

  class ErrorLogger { // singleton
    static ErrorLogger instance;
    std::fstream logFile;
    ErrorLogger() {}
    ErrorLogger(const ErrorLogger &) {};
  public:
    ~ErrorLogger();
    static ErrorLogger & getInstance() { return instance; }
    static void open(const char * name);
    static void close() { instance.logFile.close(); }
    template <typename T> 
    ErrorLogger & operator<<(const T & t) {
      logFile << t;
      std::clog << t;
      BasicLog << t;
      return *this;
    }
    ErrorLogger & operator<<(std::ostream & (*manip)(std::ostream &));
  };

/***********************************************************
 * Exceptions
 ***********************************************************/
  class Exception : public std::exception {
    std::ostringstream msg;
  public:
    virtual const char * what() const throw() { return msg.str().c_str(); }
    virtual ~Exception() throw() {};
    template <typename T> Exception & operator<<(const T & t) {
      ErrorLog << t;
      msg << t;
      return *this;
    }
    Exception & operator<<(std::ostream & (*manip)(std::ostream &));
    // default constructor
    Exception() {};
    // copy constructor
    Exception(Exception & ex) { msg << ex.msg.str(); }
    // To use like MyException(AT)
    Exception(const char * a, const char * b, const char * c);
  };

  class Crash : public Exception {
  public:
    Crash(const char * a, const char * b, const char * c): Exception(a,b,c) {}
  };

} /* namespace Debug */

#endif /* DEBUG_HPP_ */
