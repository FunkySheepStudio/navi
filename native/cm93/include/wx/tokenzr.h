#pragma once
#include "wx.h"
#include <vector>

class wxStringTokenizer {
public:
  wxStringTokenizer(const std::string& s, const std::string& delim) {
    std::string tmp;
    for(char c: s) {
      if(delim.find(c)!=std::string::npos) { if(!tmp.empty()){ tokens.push_back(tmp); tmp.clear(); } }
      else tmp.push_back(c);
    }
    if(!tmp.empty()) tokens.push_back(tmp);
    idx = 0;
  }
  wxString GetNextToken() { if(idx<tokens.size()) return wxString(tokens[idx++]); return wxString(""); }
  size_t CountTokens() const { return tokens.size(); }
private:
  std::vector<std::string> tokens;
  size_t idx = 0;
};
