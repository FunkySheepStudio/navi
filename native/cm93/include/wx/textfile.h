#pragma once
#include "wx.h"
#include <vector>

class wxTextFile {
public:
  bool Open(const std::string&) { return false; }
  size_t GetLineCount() const { return lines.size(); }
  std::string GetLine(size_t i) const { return (i<lines.size()? lines[i] : std::string()); }
  void Close() {}
  void AddLine(const std::string &l) { lines.push_back(l); }
private:
  std::vector<std::string> lines;
};
