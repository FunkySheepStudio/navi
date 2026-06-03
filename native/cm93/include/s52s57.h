// s52s57.h - minimal stub
#pragma once
#include "ocpn_stubs.h"

class wxArrayOfS57attVal {
public:
  void Add(S57attVal *v) {
    if (v) values.push_back(v);
  }
  int GetCount() const { return static_cast<int>(values.size()); }
  S57attVal *&operator[](size_t idx) { return values[idx]; }
  std::vector<S57attVal *> values;
};

class wxArrayPtrVoid {
public:
  void Add(void *p) { if (p) values.push_back(p); }
  int GetCount() const { return static_cast<int>(values.size()); }
  std::vector<void *> values;
};

struct pt {
  double x = 0.0;
  double y = 0.0;
};

inline void s52_init() {}
