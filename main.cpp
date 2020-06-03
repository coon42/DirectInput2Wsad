#include "input.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------------------
// main
//-------------------------------------------------------------------------------------------------------------

int main(const char* pArgs, int argc) {
  // TODO: move Config to Application class:
  Config config("config.ini");

  Application app;
  app.run();
}
