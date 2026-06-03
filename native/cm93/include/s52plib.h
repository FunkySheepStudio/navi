// s52plib.h - minimal stub
#pragma once
#include "ocpn_stubs.h"

class s52plib {
public:
  int m_nSymbolStyle = PAPER_CHART;
  int m_nBoundaryStyle = PLAIN_BOUNDARIES;
  bool m_b2pointLUPS = false;
  bool m_b2lineLUPS = false;

  LUPrec* S52_LUPLookup(LUPname name, const wxString &feature, S57Obj *obj) {
    (void)name;
    (void)feature;
    (void)obj;
    return nullptr;
  }

  void _LUP2rules(LUPrec *rules, S57Obj *obj) {
    (void)rules;
    (void)obj;
  }
};
extern s52plib* ps52plib;
inline s52plib* ps52plib = new s52plib();
