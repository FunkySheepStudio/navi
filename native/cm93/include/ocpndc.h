// ocpndc.h - minimal stub
#pragma once
#include "ocpn_stubs.h"

class OCPNPlatform {
public:
  std::string GetPrivateDataDir() const { return std::string("."); }
};

extern OCPNPlatform* g_Platform;
inline OCPNPlatform* g_Platform = new OCPNPlatform();
