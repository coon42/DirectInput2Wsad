#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>

#include "inih/cpp/INIReader.h"

//-------------------------------------------------------------------------------------------------------------
// Config
//-------------------------------------------------------------------------------------------------------------

struct ButtonConfig {
  BYTE vKey{0};
  BYTE bScan{0};
  DWORD dwFlags{0};
};

class Config {
public:
  Config(const std::string& configPath);
  const ButtonConfig readButton(const std::string& buttonName) const;

private:
  void createDefaultConfig();

  const std::string configPath_;
  INIReader reader;
};

#endif // CONFIG_H
