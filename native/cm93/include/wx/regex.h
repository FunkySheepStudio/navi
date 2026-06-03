#pragma once
#include "wx.h"

class wxRegEx {
public:
  wxRegEx(const std::string&) {}
  bool Matches(const std::string&) const { return false; }
};
