#include <string>

namespace Debug {
  class Logger;
}

namespace Globals {
  extern std::string progName; 
  extern std::string progDir; 

  void On(void);
  void Off(void);
} /* namespace Globals */

