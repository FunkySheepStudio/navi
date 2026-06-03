// s57chart.h - minimal stub
#pragma once
#include "ocpn_stubs.h"

class s57chart {
public:
  virtual ~s57chart() {}
  // Common fields used by cm93chart and other chart types
  double ref_lat = 0.0;
  double ref_lon = 0.0;
  int m_ChartType = 0;
  chart_context* m_this_chart_context = nullptr;
  bool m_RAZBuilt = false;
  double m_easting_vp_center = 0.0;
  double m_northing_vp_center = 0.0;
  int m_pixx_vp_center = 0;
  int m_pixy_vp_center = 0;
  double m_view_scale_ppm = 1.0;
  bool m_b2pointLUPS = false;
  bool m_b2lineLUPS = false;
  int m_next_safe_cnt = 0;

  // Methods
  static std::list<S57Obj*> *GetAssociatedObjects(S57Obj *obj) { (void)obj; return nullptr; }
};

struct S57Obj {
  double x_rate=0, x_origin=0, y_rate=0, y_origin=0;
  struct BBObj_t { double lon_min=0, lat_min=0, lon_max=0, lat_max=0; double GetMinLon() const { return lon_min; } double GetMaxLon() const { return lon_max; } double GetMinLat() const { return lat_min; } double GetMaxLat() const { return lat_max; } void Set(double a, double b, double c, double d) { lon_min = a; lat_min = b; lon_max = c; lat_max = d; } void EnLarge(double d) { lon_min -= d; lat_min -= d; lon_max += d; lat_max += d; } } BBObj;
  chart_context* m_chart_context = nullptr;

  // Additional members used by cm93 rendering
  double m_lat = 0.0, m_lon = 0.0;
  int Primitive_type = 0;
  char FeatureName[8] = {'\0'};
  int Index = 0;
  wxArrayOfS57attVal *attVal = nullptr;
  char *att_array = nullptr;
  int n_attr = 0;
  int npt = 0;
  pt *geoPt = nullptr;
  double *geoPtz = nullptr;
  double *geoPtMulti = nullptr;
  void *pPolyTessGeo = nullptr;
  bool m_bcategory_mutable = false;
  int iOBJL = -1;
  int m_n_lsindex = 0;
  int *m_lsindex_array = nullptr;
  int m_n_edge_max_points = 0;
  bool bIsAton = false;
  bool bIsAssociable = false;
  int m_DisplayCat = 0;
  int m_DPRI = 0;
  int nRef = 0;
  double x = 0.0;
  double y = 0.0;
  int auxParm3 = 0; // chart type marker used by cm93 port

  // Methods
  wxString GetAttrValueAsString(int idx) { (void)idx; return wxString(""); }
  wxString GetAttrValueAsString(const char *attr) { (void)attr; return wxString(""); }
  int GetAttributeIndex(const char *attr) const { (void)attr; return -1; }
  int Len() const { return 0; }
};

struct ObjRazRules {
  S57Obj *obj = nullptr;
  ObjRazRules *next = nullptr;
  void *dummy1 = nullptr;  // for alignment
  void *dummy2 = nullptr;
};

inline ObjRazRules *razRules[PI_PRIO_NUM][PI_LUPNAME_NUM] = {};
