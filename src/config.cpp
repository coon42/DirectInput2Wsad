#include "config.h"

//-------------------------------------------------------------------------------------------------------------
// Config
//-------------------------------------------------------------------------------------------------------------

Config::Config(const std::string& configPath) : configPath_(configPath), reader(configPath) {
  createDefaultConfig(); // TODO: only create default config when config file does not already exist.
}

void Config::createDefaultConfig() {
  // TODO: implement
}

const ButtonConfig Config::readButton(const std::string& buttonName) const {
  if (reader.ParseError() < 0)
    throw "Can't load ini file!\n";

  const std::string category = "DualShock2";

  ButtonConfig cfg;
  cfg.vKey = static_cast<BYTE>(reader.GetInteger(category, buttonName + "_key", 0));
  cfg.bScan = static_cast<BYTE>(reader.GetInteger(category, buttonName + "_scan", 0));
  cfg.dwFlags = static_cast<DWORD>(reader.GetInteger(category, buttonName + "_flags", 0));

  return cfg;
}
