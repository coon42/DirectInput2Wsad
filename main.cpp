#include "input.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------------------
// main
//-------------------------------------------------------------------------------------------------------------

int main(const char* pArgs, int argc) {
  // TODO: move Config to Input class:
  Config config("config.ini");

  Input input;  
  input.processOld();
}
