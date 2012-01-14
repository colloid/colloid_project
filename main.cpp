#include <iostream>
#include "globals.hpp"
#include "debug.hpp"

using std::endl;
using namespace Globals;

void func(int a) {
  CRASH << a << " Тестовая надпись" << endl;
}

void func2(long a) {
  func(a - 1);
}

int main(int argc, char ** argv) {
  Globals::progName = argv[0];
  TraceLog << progName << std::endl;
  Globals::On();
  try {
    func2(2);
    Globals::Off();
    return 0;
  } catch (Debug::Exception & ex) {
    ErrorLog << "CRASH: program crashed due to uncaught exception." << endl;
    std::terminate();
  } catch (std::exception & ex) {
    ErrorLog << "CRASH: program crashed due to uncaught exception: "
	     << ex.what() << endl;
    std::terminate();
  } catch (...) {
    ErrorLog << "CRASH: program crashed due to uncaught unknown exception" << endl;
    std::terminate();
  }
}
