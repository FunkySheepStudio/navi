// Minimal stubs to allow incremental compilation of extracted OpenCPN CM93 sources.
#pragma once

#ifndef OCPN_USE_WX_STUBS
#define OCPN_USE_WX_STUBS 1
#endif

#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <cstdarg>
#include <cstdio>
#include <cctype>
#include <fstream>
#include <list>
#include <unordered_map>

// Forward declare S57 and geometry objects used by CM93 ported code
class S57Obj;
class wxArrayOfS57attVal;
struct wxString;
struct pt;
class OCPNRegion;
class wxFont;
class wxSizer;

struct wxPoint {
  int x = 0;
  int y = 0;
  wxPoint() = default;
  wxPoint(int a, int b) : x(a), y(b) {}
};

using wxWindowID = int;

struct wxSize {
  wxSize() = default;
  wxSize(int w, int h) : width(w), height(h), x(w), y(h) {}
  int width = 0;
  int height = 0;
  int x = 0;
  int y = 0;
};

class wxWindow {
public:
  void Destroy() {}
  void Disable() {}
  void Show() {}
  void Enable() {}
  void SetFont(const wxFont &font) { (void)font; }
  void SetSizer(wxSizer *sizer) { (void)sizer; }
  int GetCharWidth() const { return 8; }
  int GetCharHeight() const { return 16; }
  void Refresh(bool eraseBackground = true) { (void)eraseBackground; }
  void Hide() {}
  void Close() {}
  void Centre() {}
  wxSize GetSize() const { return size_; }
  void SetSize(const wxSize &size) { size_ = size; size_.x = size.width; size_.y = size.height; }
  void SetSize(int w, int h) { size_.width = w; size_.height = h; size_.x = w; size_.y = h; }
  template <typename Handler>
  bool Connect(int event, Handler handler, void *userData, void *thisPtr) {
    (void)event;
    (void)handler;
    (void)userData;
    (void)thisPtr;
    return true;
  }
private:
  wxSize size_;
};

class wxDirTraverser;
class wxArrayString;
class ChartCanvas;

#define _(x) x

class wxDialog : public wxWindow {
public:
  bool Create(wxWindow *parent, wxWindowID id, const wxString &title,
              const wxPoint &pos, const wxSize &size, long style) {
    (void)parent; (void)id; (void)title; (void)pos; (void)size; (void)style;
    return true;
  }
};

class wxListCtrl;

#define IMPLEMENT_CLASS(cls, base)
#define BEGIN_EVENT_TABLE(cls, base)
#define EVT_CLOSE(func)
#define END_EVENT_TABLE()

const long wxDEFAULT_DIALOG_STYLE = 0;
const long wxRESIZE_BORDER = 0;
const long wxLC_REPORT = 0;
const long wxLC_SINGLE_SEL = 0;
const long wxLC_HRULES = 0;
const long wxLC_VRULES = 0;
const long wxBORDER_SUNKEN = 0;
const long wxLC_VIRTUAL = 0;
const long wxLIST_FORMAT_LEFT = 0;
const long wxLIST_FORMAT_CENTER = 1;
const long wxEVT_COMMAND_LIST_ITEM_SELECTED = 1001;
const long wxEVT_COMMAND_SPINCTRL_UPDATED = 1002;
const long wxEVT_COMMAND_BUTTON_CLICKED = 1003;
const long wxLIST_STATE_SELECTED = 1;
const long wxOutRegion = 0;
const long wxEXPAND = 0x0100;
const long wxALL = 0x0200;
const long wxVERTICAL = 0x04;
const long wxHORIZONTAL = 0x08;
const long wxID_ANY = -1;
const long wxSP_ARROW_KEYS = 0x0001;
inline const wxPoint wxDefaultPosition = wxPoint(0, 0);
inline const wxSize wxDefaultSize = wxSize(0, 0);

#define wxListEventHandler(func) (&func)
#define wxCommandEventHandler(func) (&func)

typedef char wxChar;
struct wxConvUTF8;

#ifndef wxNOT_FOUND
#define wxNOT_FOUND -1
#endif

class wxCharBuffer {
public:
  wxCharBuffer() : ptr(nullptr) {}
  wxCharBuffer(const char *p) : ptr(p) {}
  const char *data() const { return ptr; }
  operator const char *() const { return ptr; }
private:
  const char *ptr = nullptr;
};

// Simple wxString substitute with std::string interop
struct wxString {
  std::string s;
  wxString() = default;
  wxString(const char* t): s(t ? t : "") {}
  wxString(const std::string& t): s(t) {}
  wxString(char c) { s.push_back(c); }
  wxString(const wxString& t, const wxConvUTF8& conv) : s(t.s) { (void)conv; }
  bool IsEmpty() const { return s.empty(); }
  wxString& Append(const wxString& o) { s += o.s; return *this; }
  wxString& Append(const std::string& o) { s += o; return *this; }
  wxString& Append(const char* o) { s += (o ? o : ""); return *this; }
  wxString& operator+=(const std::string& o) { s += o; return *this; }
  wxString& operator+=(const wxString& o) { s += o.s; return *this; }
  wxString& operator+=(const char* o) { s += (o ? o : ""); return *this; }
  wxString& operator=(const std::string& o) { s = o; return *this; }
  operator std::string() const { return s; }
  operator const char*() const { return s.c_str(); }
  const char* mb_str() const { return s.c_str(); }
  char Last() const { return s.empty() ? '\0' : s.back(); }
  void Replace(const std::string& a, const std::string& b) { size_t p = 0; while((p = s.find(a, p))!=std::string::npos){ s.replace(p,a.size(),b); p+=b.size(); }}
  const char* c_str() const { return s.c_str(); }
  bool Empty() const { return s.empty(); }
  bool StartsWith(const wxString &p) const { return s.rfind(p.s, 0) == 0; }
  wxString Mid(size_t pos, size_t len = std::string::npos) const { return wxString(s.substr(pos, len)); }
  wxString BeforeLast(const wxString &delimiter) const { if (delimiter.s.empty()) return *this; size_t pos = s.find_last_of(delimiter.s); return wxString(pos==std::string::npos ? "" : s.substr(0,pos)); }
  wxString BeforeLast(char c) const { size_t pos = s.find_last_of(c); return wxString(pos==std::string::npos ? "" : s.substr(0,pos)); }
  wxString AfterLast(const wxString &delimiter) const { if (delimiter.s.empty()) return *this; size_t pos = s.find_last_of(delimiter.s); return wxString(pos==std::string::npos ? "" : s.substr(pos + 1)); }
  wxString AfterLast(char c) const { size_t pos = s.find_last_of(c); return wxString(pos==std::string::npos ? "" : s.substr(pos + 1)); }
  bool Contains(const wxString &o) const { return s.find(o.s) != std::string::npos; }
  bool Contains(const char *o) const { if(!o) return false; return s.find(std::string(o)) != std::string::npos; }
  int Find(const wxString &o) const { size_t p = s.find(o.s); return p == std::string::npos ? wxNOT_FOUND : static_cast<int>(p); }
  int Find(const char *o) const { if(!o) return wxNOT_FOUND; size_t p = s.find(std::string(o)); return p == std::string::npos ? wxNOT_FOUND : static_cast<int>(p); }
  bool ToLong(long *out) const { try { *out = std::stol(s); return true; } catch(...) { return false; } }
  wxString Trim() const { size_t a = s.find_first_not_of(" \t\r\n"); if(a==std::string::npos) return wxString(""); size_t b = s.find_last_not_of(" \t\r\n"); return wxString(s.substr(a, b-a+1)); }
  bool IsSameAs(const wxString &o) const { return s==o.s; }
  bool IsSameAs(const char* o) const { if(!o) return false; return s == o; }
  wxString Lower() const { std::string r = s; for (char &c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c))); return wxString(r); }
  static wxString Format(const char *fmt, ...) { char buf[512]; va_list ap; va_start(ap, fmt); vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap); return wxString(buf); }
  void Printf(const char *fmt, ...) { char buf[512]; va_list ap; va_start(ap, fmt); vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap); s = buf; }
  wxString& Prepend(const wxString &o) { s = o.s + s; return *this; }
  wxString& Prepend(const char *o) { s = std::string(o ? o : "") + s; return *this; }
  int Len() const { return static_cast<int>(s.size()); }
  wxCharBuffer ToUTF8() const { return wxCharBuffer(s.c_str()); }
  void Clear() { s.clear(); }
  void Truncate(size_t len) { if (len < s.size()) s.resize(len); }
  char operator[](size_t i) const { return s[i]; }
  char& operator[](size_t i) { return s[i]; }
  wxString& operator<<(wxChar c) { s.push_back(c); return *this; }
  wxString& operator<<(const wxString &o) { s += o.s; return *this; }
  wxString& operator<<(const char *o) { s += (o ? o : ""); return *this; }
  wxString& append(const wxString &o) { s += o.s; return *this; }
  wxString& append(const char *o) { s += (o ? o : ""); return *this; }
  int Length() const { return Len(); }
};

inline const wxString wxEmptyString = wxString("");

// ostream insertion for wxString
inline std::ostream &operator<<(std::ostream &os, const wxString &ws) {
  os << ws.c_str();
  return os;
}

class wxEvent {
public:
  virtual ~wxEvent() = default;
};

class wxCommandEvent : public wxEvent {
public:
  int GetInt() const { return value_; }
  void SetInt(int value) { value_ = value; }
private:
  int value_ = 0;
};

class wxCloseEvent : public wxEvent {};
class wxListEvent : public wxEvent {
public:
  int GetIndex() const { return index_; }
  void SetIndex(int index) { index_ = index; }
private:
  int index_ = 0;
};

class wxListItem {
public:
  void SetId(int id) { id_ = id; }
  void SetColumn(int col) { column_ = col; }
  void SetText(const wxString &text) { text_ = text; }
  int GetId() const { return id_; }
  int GetColumn() const { return column_; }
  const wxString &GetText() const { return text_; }
private:
  int id_ = 0;
  int column_ = 0;
  wxString text_;
};

class wxListCtrl : public wxWindow {
public:
  wxListCtrl(wxWindow *parent, wxWindowID id, const wxPoint &pos,
             const wxSize &size, long style) {
    (void)parent;
    (void)id;
    (void)pos;
    (void)size;
    (void)style;
  }

  int InsertColumn(int col, const wxString &heading, int format, int width) {
    (void)heading;
    (void)format;
    (void)width;
    return col;
  }

  int InsertItem(const wxListItem &item) {
    (void)item;
    return item.GetId();
  }

  bool SetItem(const wxListItem &item) {
    (void)item;
    return true;
  }

  void SetItemCount(int count) { (void)count; }
  int GetItemCount() const { return item_count_; }
  void DeleteAllItems() { item_count_ = 0; }
  void SetItemState(int index, long state, long mask) {
    (void)index;
    (void)state;
    (void)mask;
  }
  bool IsVirtual() const { return false; }
  void Refresh(bool eraseBackground = true) { wxWindow::Refresh(eraseBackground); }
private:
  int item_count_ = 0;
};

class wxStaticText : public wxWindow {
public:
  wxStaticText(wxWindow *parent, wxWindowID id, const wxString &label,
               const wxPoint &pos, const wxSize &size, long style) {
    (void)parent;
    (void)id;
    (void)label;
    (void)pos;
    (void)size;
    (void)style;
  }
};

class wxSpinCtrl : public wxWindow {
public:
  wxSpinCtrl(wxWindow *parent, wxWindowID id, const wxString &value,
             const wxPoint &pos, const wxSize &size, long style, int minVal,
             int maxVal, int initial)
      : value_(initial) {
    (void)parent;
    (void)id;
    (void)value;
    (void)pos;
    (void)size;
    (void)style;
    (void)minVal;
    (void)maxVal;
  }

  int GetValue() const { return value_; }
  void SetValue(int value) { value_ = value; }
private:
  int value_ = 0;
};

class wxButton : public wxWindow {
public:
  wxButton(wxWindow *parent, wxWindowID id, const wxString &label,
           const wxPoint &pos, const wxSize &size, long style) {
    (void)parent;
    (void)id;
    (void)label;
    (void)pos;
    (void)size;
    (void)style;
  }

  void SetDefault() {}
};

class wxSizer {
public:
  virtual ~wxSizer() = default;
  virtual void Add(wxWindow *window, int proportion = 0, int flag = 0,
                   int border = 0) {
    (void)window;
    (void)proportion;
    (void)flag;
    (void)border;
  }
  virtual void Add(wxSizer *sizer, int proportion = 0, int flag = 0,
                   int border = 0) {
    (void)sizer;
    (void)proportion;
    (void)flag;
    (void)border;
  }
  void AddSpacer(int size) { (void)size; }
  void Layout() {}
};

class wxBoxSizer : public wxSizer {
public:
  wxBoxSizer(int orient) { (void)orient; }
};

inline void SetFont(wxFont &) {}
inline void SetSizer(wxSizer *) {}
inline void DimeControl(wxWindow *) {}

class wxFont {};
inline wxFont *GetOCPNScaledFont(const wxString &) { return new wxFont(); }

enum OGRatt_t {
  OGR_INT,
  OGR_INT_LST,
  OGR_REAL,
  OGR_REAL_LST,
  OGR_STR,
};

struct S57attVal {
  void *value = nullptr;
  OGRatt_t valType = OGR_INT;
};

enum DisCat {
  DISPLAYBASE = 'D',
  STANDARD = 'S',
  OTHER = 'O',
  MARINERS_STANDARD = 'M',
  MARINERS_OTHER,
  DISP_CAT_NUM,
};

enum LUPname {
  SIMPLIFIED = 'L',
  PAPER_CHART = 'R',
  LINES = 'S',
  PLAIN_BOUNDARIES = 'N',
  SYMBOLIZED_BOUNDARIES = 'O',
  LUPNAME_NUM = 5,
};

struct LUPrec {
  int RCID = 0;
  char OBCL[7] = {};
  int FTYP = 0;
  int DPRI = 0;
  DisCat DISC = DISPLAYBASE;
  LUPname TNAM = SIMPLIFIED;
  std::vector<std::string> ATTArray;
  wxString INST;
  int LUCM = 0;
  int nSequence = 0;
  void *ruleList = nullptr;
};

struct VE_Element;
struct VC_Element;

// Minimal chart_context placeholder used in cm93chart
struct chart_context {
  void *chart = nullptr;
  void *m_pvc_hash = nullptr;
  void *m_pve_hash = nullptr;
  void *pFloatingATONArray = nullptr;
  void *pRigidATONArray = nullptr;
  int chart_type = 0;
  double safety_contour = 0.0;
  void *vertex_buffer = nullptr;
  std::list<S57Obj*> *(*pt2GetAssociatedObjects)(S57Obj*) = nullptr;

  // Methods for rendering
  void *Get_vc_hash() { return m_pvc_hash; }
  void *Get_ve_hash() { return m_pve_hash; }
  int GetChartType() { return chart_type; }
  void *GetLineVertexBuffer() { return vertex_buffer; }
};



struct float_2Dpt { float x; float y; };

struct LLBBox {
  double lat_min, lon_min, lat_max, lon_max;
  void Set(double a, double b, double c, double d) { lat_min = a; lon_min = b; lat_max = c; lon_max = d; }
  bool IntersectOut(const LLBBox &other) const { (void)other; return false; }
  double GetMinLon() const { return lon_min; }
  double GetMaxLon() const { return lon_max; }
  double GetMinLat() const { return lat_min; }
  double GetMaxLat() const { return lat_max; }
};

class OGRGeometry {
public:
  virtual ~OGRGeometry() = default;
};

class OGRPoint : public OGRGeometry {
public:
  OGRPoint(double x, double y, double z) : x_(x), y_(y), z_(z) {}
  double getX() const { return x_; }
  double getY() const { return y_; }
  double getZ() const { return z_; }
private:
  double x_ = 0;
  double y_ = 0;
  double z_ = 0;
};

class PolyTessGeo {
public:
  PolyTessGeo(void *xgeom) { (void)xgeom; }
};

class OGRMultiPoint : public OGRGeometry {
public:
  void addGeometryDirectly(OGRPoint *g) { if (g) geoms_.push_back(g); }
  int getNumGeometries() const { return static_cast<int>(geoms_.size()); }
  OGRGeometry* getGeometryRef(int i) { return (i >= 0 && i < static_cast<int>(geoms_.size())) ? geoms_[i] : nullptr; }
private:
  std::vector<OGRPoint*> geoms_;
};

// Point-in-polygon test for float vertex arrays (lon/lat pairs)
inline bool G_PtInPolygon_FL(const float *pvertices, int nvertices, double x, double y) {
  if (!pvertices || nvertices < 3) return false;
  bool inside = false;
  for (int i = 0, j = nvertices - 1; i < nvertices; j = i++) {
    double xi = pvertices[2*i+0];
    double yi = pvertices[2*i+1];
    double xj = pvertices[2*j+0];
    double yj = pvertices[2*j+1];
    bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi + 1e-15) + xi);
    if (intersect) inside = !inside;
  }
  return inside;
}

// Overload accepting float_2Dpt* as used by cm93 code
inline bool G_PtInPolygon_FL(float_2Dpt *pvertices, int nvertices, double x, double y) {
  if (!pvertices) return false;
  // Convert to temporary float array view
  std::vector<float> verts;
  verts.reserve(nvertices * 2);
  for (int i = 0; i < nvertices; ++i) {
    verts.push_back(pvertices[i].x);
    verts.push_back(pvertices[i].y);
  }
  return G_PtInPolygon_FL(verts.data(), nvertices, x, y);
}


// Forward declare projection helpers (defined later)
inline double toSM(double lat, double lon, double clat, double clon, double* easting, double* northing);
inline void fromSM(double easting, double northing, double clat, double clon, double *plat, double *plon);
struct wxPoint2DDouble { double m_x = 0.0; double m_y = 0.0; double x = 0.0; double y = 0.0; wxPoint2DDouble(){} wxPoint2DDouble(double a,double b):m_x(a),m_y(b),x(a),y(b){} wxPoint2DDouble operator-(const wxPoint2DDouble &o) const { return wxPoint2DDouble(m_x - o.m_x, m_y - o.m_y); } };



class OCPNRegion {
public:
  OCPNRegion() : x(0), y(0), width(0), height(0) {}
  OCPNRegion(int x_, wxPoint *p) : x(x_), y(0), width(0), height(0) { (void)p; }
  OCPNRegion(int a, int b, int c, int d) : x(a), y(b), width(c), height(d) {}
  void Clear() { x = y = width = height = 0; }
  bool IsEmpty() const { return width <= 0 || height <= 0; }
  bool IsOk() const { return width > 0 && height > 0; }
  void Union(const OCPNRegion &r) { (void)r; }
  void Intersect(const OCPNRegion &r) { (void)r; }
  void Subtract(const OCPNRegion &o) { (void)o; }
  bool Empty() const { return IsEmpty(); }
  bool Contains(const wxPoint &p) const {
    return !IsEmpty() && p.x >= x && p.x < x + width && p.y >= y && p.y < y + height;
  }
  int Contains(int x0, int y0, int w, int h) const {
    if (IsEmpty() || w <= 0 || h <= 0) return wxOutRegion;
    if (x0 + w <= x || x0 >= x + width || y0 + h <= y || y0 >= y + height)
      return wxOutRegion;
    return 1; // partial or inside
  }
  int GetX() const { return x; }
  int GetY() const { return y; }
  int GetWidth() const { return width; }
  int GetHeight() const { return height; }
  int x;
  int y;
  int width;
  int height;
};

class wxDateTime {
public:
  wxDateTime() = default;
  bool ParseFormat(const wxString &s, const char *fmt) {
    valid = false;
    if (!fmt || !s.Len()) return false;
    std::string str = s;
    if (strcmp(fmt, "%Y%m%d") == 0 && str.size() == 8) {
      year = std::stoi(str.substr(0, 4));
      month = std::stoi(str.substr(4, 2));
      day = std::stoi(str.substr(6, 2));
      valid = true;
      return true;
    }
    return false;
  }
  bool IsValid() const { return valid; }
  bool IsLaterThan(const wxDateTime &o) const {
    if (!valid) return false;
    if (!o.valid) return true;
    if (year != o.year) return year > o.year;
    if (month != o.month) return month > o.month;
    return day > o.day;
  }
private:
  int year = 0;
  int month = 0;
  int day = 0;
  bool valid = false;
};

class ViewPort {
public:
  int pix_width = 800;
  int pix_height = 600;
  double clat = 0.0;
  double clon = 0.0;
  double view_scale_ppm = 1.0;
  double rotation = 0.0;
  int m_projection_type = 0;
  int chart_scale = 0;
  bool b_quilt = false;
  bool b_FullScreenQuilt = false;
  void SetRotationAngle(double r) { rotation = r; }
  OCPNRegion rv_rect;
  bool IsValid() const { return true; }
  OCPNRegion GetVPRegionIntersect() const { return OCPNRegion(); }
  OCPNRegion GetVPRegionIntersect(const OCPNRegion &ScreenRegion, int nvertices, float *pvertices, int chart_native_scale, wxPoint *DrawBuf) const {
    (void)ScreenRegion; (void)nvertices; (void)pvertices; (void)chart_native_scale; (void)DrawBuf; return OCPNRegion();
  }
  wxPoint GetPixFromLL(double lat, double lon) const {
    double e,n; toSM(lat, lon, clat, clon, &e, &n);
    int px = int(std::round(e / view_scale_ppm + pix_width/2));
    int py = int(std::round(pix_height/2 - (n / view_scale_ppm)));
    return wxPoint(px, py);
  }
  wxPoint2DDouble GetDoublePixFromLL(double lat, double lon) const {
    double e,n; toSM(lat, lon, clat, clon, &e, &n);
    double px = (e / view_scale_ppm) + (pix_width/2);
    double py = (pix_height/2) - (n / view_scale_ppm);
    return wxPoint2DDouble(px, py);
  }
  void GetLLFromPix(const wxPoint &p, double *plat, double *plon) const {
    double e = (p.x - pix_width/2) * view_scale_ppm;
    double n = (pix_height/2 - p.y) * view_scale_ppm;
    fromSM(e, n, clat, clon, plat, plon);
  }
  LLBBox GetBBox() const { LLBBox b; b.Set(clat - 1, clon - 1, clat + 1, clon + 1); return b; }
  bool operator==(const ViewPort &other) const {
    return clat == other.clat && clon == other.clon && pix_width == other.pix_width && pix_height == other.pix_height && view_scale_ppm == other.view_scale_ppm && m_projection_type == other.m_projection_type;
  }
};

class wxColour {
public:
  wxColour() = default;
  wxColour(const wxString &s) { setFromName(s); }
  wxColour(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
      : r(r_), g(g_), b(b_), a(a_) {}

  uint8_t Red() const { return r; }
  uint8_t Green() const { return g; }
  uint8_t Blue() const { return b; }
  uint8_t Alpha() const { return a; }
  bool IsOk() const { return a != 0 || r != 0 || g != 0 || b != 0; }

private:
  void setFromName(const wxString &name) {
    std::string s = name.c_str();
    for (char &c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    if (s == "BLACK") { r = g = b = 0; a = 255; }
    else if (s == "WHITE") { r = g = b = 255; a = 255; }
    else if (s == "YELLOW") { r = 255; g = 255; b = 0; a = 255; }
    else if (s == "RED") { r = 255; g = 0; b = 0; a = 255; }
    else if (s == "GREEN") { r = 0; g = 255; b = 0; a = 255; }
    else if (s == "BLUE") { r = 0; g = 0; b = 255; a = 255; }
    else { r = g = b = 0; a = 255; }
  }

  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 255;
};

class wxBrush {
public:
  wxBrush() = default;
  wxBrush(const wxColour &colour) : colour(colour) {}
  wxColour GetColour() const { return colour; }
private:
  wxColour colour;
};

using wxDash = int;

enum wxPenStyle { wxPENSTYLE_USER_DASH = 1, wxPENSTYLE_LONG_DASH = 2 };

class wxPen {
public:
  wxPen() = default;
  wxPen(const wxColour &color, int width) : color(color), width(width) {}
  wxPen(const wxColour &color, int width, wxPenStyle style)
      : color(color), width(width), style(style) {}
  void SetStyle(int style) { (void)style; }
  void SetDashes(int n, const wxDash *dashes) { (void)n; (void)dashes; }
  wxColour color;
  int width = 0;
  wxPenStyle style = wxPENSTYLE_USER_DASH;
};

class wxBitmap {
public:
  wxBitmap() = default;
  wxBitmap(int w, int h) : width(w), height(h), depth(32) {
    pixels_.assign(static_cast<size_t>(w) * static_cast<size_t>(h) * 4, 0);
  }
  wxBitmap(int w, int h, int depth_) : width(w), height(h), depth(depth_) {
    pixels_.assign(static_cast<size_t>(w) * static_cast<size_t>(h) * 4, 0);
  }

  bool IsOk() const { return width > 0 && height > 0 && !pixels_.empty(); }
  int GetWidth() const { return width; }
  int GetHeight() const { return height; }
  uint8_t* GetPixels() { return pixels_.empty() ? nullptr : pixels_.data(); }
  const uint8_t* GetPixels() const { return pixels_.empty() ? nullptr : pixels_.data(); }
  void Clear(const wxColour &colour) {
    if (!IsOk()) return;
    for (size_t i = 0; i < pixels_.size(); i += 4) {
      pixels_[i + 0] = colour.Red();
      pixels_[i + 1] = colour.Green();
      pixels_[i + 2] = colour.Blue();
      pixels_[i + 3] = colour.Alpha();
    }
  }

  int width = 0;
  int height = 0;
  int depth = 0;

private:
  std::vector<uint8_t> pixels_;
};

class wxMemoryDC {
public:
  void SetPen(const class wxPen &pen) { pen_ = pen; }
  void SetBrush(const class wxBrush &brush) { brush_ = brush; }
  void SetBackground(const class wxBrush &brush) { background_ = brush; }

  void DrawLine(int x0, int y0, int x1, int y1) {
    if (!bitmap_ || !bitmap_->IsOk()) return;
    const auto setPixel = [&](int x, int y) {
      if (x < 0 || x >= bitmap_->GetWidth() || y < 0 || y >= bitmap_->GetHeight()) return;
      uint8_t *pixels = bitmap_->GetPixels();
      size_t idx = (static_cast<size_t>(y) * bitmap_->GetWidth() + static_cast<size_t>(x)) * 4;
      pixels[idx + 0] = pen_.color.Red();
      pixels[idx + 1] = pen_.color.Green();
      pixels[idx + 2] = pen_.color.Blue();
      pixels[idx + 3] = pen_.color.Alpha();
    };
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
      setPixel(x0, y0);
      if (x0 == x1 && y0 == y1) break;
      int e2 = 2 * err;
      if (e2 >= dy) { err += dy; x0 += sx; }
      if (e2 <= dx) { err += dx; y0 += sy; }
    }
  }

  void DrawLines(int n, const wxPoint *pts, int x, int y, bool close) {
    if (!pts || n <= 0) return;
    wxPoint prev = pts[0];
    for (int i = 1; i < n; ++i) {
      DrawLine(prev.x + x, prev.y + y, pts[i].x + x, pts[i].y + y);
      prev = pts[i];
    }
    if (close && n > 1) {
      DrawLine(pts[n - 1].x + x, pts[n - 1].y + y, pts[0].x + x, pts[0].y + y);
    }
  }

  void SelectObject(const wxBitmap &bmp) { bitmap_ = const_cast<wxBitmap*>(bmp.IsOk() ? &bmp : nullptr); }
  void SelectObject(wxBitmap &bmp) { bitmap_ = bmp.IsOk() ? &bmp : nullptr; }

  void Clear() {
    if (!bitmap_ || !bitmap_->IsOk()) return;
    bitmap_->Clear(background_.GetColour());
  }

  void Blit(int x, int y, int w, int h, const wxMemoryDC *src, int sx,
            int sy) {
    if (!bitmap_ || !bitmap_->IsOk() || !src || !src->bitmap_ || !src->bitmap_->IsOk()) return;
    for (int dy = 0; dy < h; ++dy) {
      int dst_y = y + dy;
      int src_y = sy + dy;
      if (dst_y < 0 || dst_y >= bitmap_->GetHeight() || src_y < 0 || src_y >= src->bitmap_->GetHeight()) continue;
      for (int dx = 0; dx < w; ++dx) {
        int dst_x = x + dx;
        int src_x = sx + dx;
        if (dst_x < 0 || dst_x >= bitmap_->GetWidth() || src_x < 0 || src_x >= src->bitmap_->GetWidth()) continue;
        size_t dst_idx = (static_cast<size_t>(dst_y) * bitmap_->GetWidth() + static_cast<size_t>(dst_x)) * 4;
        size_t src_idx = (static_cast<size_t>(src_y) * src->bitmap_->GetWidth() + static_cast<size_t>(src_x)) * 4;
        uint8_t *dst_pixels = bitmap_->GetPixels();
        const uint8_t *src_pixels = src->bitmap_->GetPixels();
        dst_pixels[dst_idx + 0] = src_pixels[src_idx + 0];
        dst_pixels[dst_idx + 1] = src_pixels[src_idx + 1];
        dst_pixels[dst_idx + 2] = src_pixels[src_idx + 2];
        dst_pixels[dst_idx + 3] = src_pixels[src_idx + 3];
      }
    }
  }

private:
  wxBitmap *bitmap_ = nullptr;
  wxPen pen_;
  wxBrush brush_;
  wxBrush background_{wxColour(0, 0, 0, 255)};
};

inline wxBitmap wxNullBitmap;

class wxColourDatabase {
public:
  wxColour Find(const wxString &name) { return wxColour(name); }
};

inline wxBrush *wxBLACK_BRUSH = new wxBrush(wxColour(0, 0, 0, 255));
inline wxColourDatabase *wxTheColourDatabase = new wxColourDatabase();

inline void wxPrintf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

class Extended_Geometry {
public:
  Extended_Geometry() = default;
  ~Extended_Geometry() = default;

  OGRGeometry *pogrGeom = nullptr;
  int n_vector_indices = 0;
  int *pvector_index = nullptr;
  int n_contours = 0;
  int *contour_array = nullptr;
  int n_max_vertex = 0;
  int pointx = 0;
  int pointy = 0;
  wxPoint2DDouble *vertex_array = nullptr;
  int xmin = 0;
  int xmax = 0;
  int ymin = 0;
  int ymax = 0;
  int n_max_edge_points = 0;

  double x_rate = 0.0;
  double x_offset = 0.0;
  double y_rate = 0.0;
  double y_offset = 0.0;
  double ref_lat = 0.0;
  double ref_lon = 0.0;
};

// LLRegion stub (lat/lon region)
class LLRegion {
public:
  LLRegion() {}
  LLRegion(unsigned int n, double *pts) { (void)n; (void)pts; }
  void Clear() {}
  void Union(const LLRegion &other) { (void)other; }
  static bool PointsCCW(unsigned int n, double *pts) { (void)n; (void)pts; return false; }
  bool Contains(double lon, double lat) const { (void)lon; (void)lat; return false; }
  bool Empty() const { return true; }
  void Subtract(const LLRegion &o) { (void)o; }
  void Intersect(const LLRegion &o) { (void)o; }
};

// Simple file output stream stubs
class wxFileOutputStream {
public:
  wxFileOutputStream(const std::string& path) : path_(path), file_(path, std::ios::binary) {}
  bool IsOk() const { return file_.is_open(); }
  int Write(const void* buffer, int size) {
    if (!file_.is_open()) return 0;
    file_.write(static_cast<const char*>(buffer), size);
    return file_.good() ? size : 0;
  }
  bool Close() { file_.close(); return true; }
private:
  std::string path_;
  std::ofstream file_;
};

// Alias for backward compatibility
using wxFFileOutputStream = wxFileOutputStream;

// Decompression stub used by ported code
inline bool DecompressXZFile(const std::string &src, const std::string &dst) { (void)src; (void)dst; return false; }

// File input stream stub  
class wxFileInputStream {
public:
  wxFileInputStream(const std::string& path) : file_(path, std::ios::binary) {}
  bool IsOk() const { return file_.is_open(); }
  wxFileInputStream& Read(void* buffer, int size) {
    if (!file_.is_open()) return *this;
    file_.read(static_cast<char*>(buffer), size);
    return *this;
  }
  bool Eof() const { return file_.eof(); }
  int GetC() {
    if (!file_.is_open()) return EOF;
    char c;
    file_.get(c);
    return file_.eof() ? EOF : (unsigned char)c;
  }
  void SeekI(std::streampos pos) { if (file_.is_open()) file_.seekg(pos); }
  bool Close() { file_.close(); return true; }
private:
  std::ifstream file_;
};

// Alias for backward compatibility
using wxFFileInputStream = wxFileInputStream;

class ColorScheme {};
class ocpnDC {
public:
  void SetVP(const ViewPort &vp) { (void)vp; }
  void SetPen(const wxPen &pen) { (void)pen; }
  void SetBrush(const wxColour &brush) { (void)brush; }
  void DrawLine(int x0, int y0, int x1, int y1) { (void)x0; (void)y0; (void)x1; (void)y1; }
  void DrawLines(int n, const wxPoint *pts, int x, int y, bool close) {
    (void)n; (void)pts; (void)x; (void)y; (void)close;
  }
};
class ocpnMemDC : public wxMemoryDC {};
struct wxRect { int x = 0; int y = 0; int width = 0; int height = 0; wxRect() = default; wxRect(int a, int b, int c, int d):x(a),y(b),width(c),height(d){};
  void Union(const wxRect &other) { if (other.x < x) x = other.x; if (other.y < y) y = other.y; width = std::max(width, other.x + other.width - x); height = std::max(height, other.y + other.height - y); }
};
class wxGLContext {};
class ThumbData {};
class OCPNRegionIterator {
public:
  explicit OCPNRegionIterator(const OCPNRegion &region) : region_(region), index_(0) { (void)region_; }
  bool HaveRects() const { return false; }
  void NextRect() { ++index_; }
  wxRect GetRect() const { return wxRect(); }
private:
  OCPNRegion region_;
  int index_ = 0;
};

enum ClipResult { Invisible = 0, Visible = 1, Partial = 2 };
inline ClipResult cohen_sutherland_line_clip_i(int *x0, int *y0, int *x1, int *y1, int xmin, int xmax, int ymin, int ymax) { (void)x0; (void)y0; (void)x1; (void)y1; (void)xmin; (void)xmax; (void)ymin; (void)ymax; return Visible; }
class ListOfObjRazRules {
public:
  void Add(S57Obj *obj) {
    if (obj) items.push_back(obj);
  }
  size_t GetCount() const { return items.size(); }
  S57Obj *operator[](size_t i) { return items[i]; }
  std::vector<S57Obj *> items;
};

inline ListOfObjRazRules *pFloatingATONArray = new ListOfObjRazRules();
inline ListOfObjRazRules *pRigidATONArray = new ListOfObjRazRules();

// Additional globals used by cm93
inline bool g_bopengl = false;
inline int g_cm93_zoom_factor = 1;
inline wxWindow *pPopupDetailSlider = nullptr;
inline ColorScheme m_global_color_scheme;

inline int GetChartType() { return 0; }
inline void *GetLineVertexBuffer() { return nullptr; }
inline void _insertRules(S57Obj *obj, LUPrec *lup, void *chart) { (void)obj; (void)lup; (void)chart; }

class S57ObjectDesc {};

// S57Obj and ObjRazRules are provided by s57chart.h; do not redefine here.

struct VE_Element {
  int index = 0;
  int nCount = 0;
  void *pPoints = nullptr;
  int max_priority = 0;
  LLBBox edgeBBox;
};
struct VC_Element {};

inline std::unordered_map<unsigned, VE_Element *> &Get_ve_hash() {
  static std::unordered_map<unsigned, VE_Element *> hash;
  return hash;
}

inline std::unordered_map<unsigned, VC_Element *> &Get_vc_hash() {
  static std::unordered_map<unsigned, VC_Element *> hash;
  return hash;
}

// Provide simple array/list macros
// Provide simple array/list macros (define safe fallbacks only if not defined)
#ifndef WX_DECLARE_OBJARRAY
#define WX_DECLARE_OBJARRAY(type, name) \
class name { \
public: \
  void Add(type* p) { if(p) v.push_back(*p); } \
  void Add(const type &t) { v.push_back(t); } \
  size_t GetCount() const { return v.size(); } \
  type& operator[](size_t i) { return v[i]; } \
private: \
  std::vector<type> v; \
};
#endif

#ifndef WX_DEFINE_ARRAY_PTR
#define WX_DEFINE_ARRAY_PTR(type, name) using name = std::vector<type>;
#endif

#ifndef WX_DECLARE_LIST
#define WX_DECLARE_LIST(type, name) using name = std::vector<type>;
#endif

class wxDialog;
class wxCloseEvent;
class wxCommandEvent;
class wxListEvent;
class wxTimer;
class wxWindow;
class wxSpinCtrl;
class wxButton;

#define DECLARE_CLASS(x)
#define DECLARE_EVENT_TABLE()

// Provide a simple wxRemoveFile helper (wxFileName is provided by ocpn_port_shim)
inline bool wxRemoveFile(const wxString &p) { return std::filesystem::remove(std::filesystem::path(p.c_str())); }

// Simple logging stub
inline void wxLogMessage(const char *m) { (void)m; }
inline void wxLogMessage(const wxString &m) { (void)m; }

// wxString concatenation operators
inline wxString operator+(const wxString &a, const wxString &b) { return wxString(std::string(a.c_str()) + b.c_str()); }
inline wxString operator+(const wxString &a, const char *b) { return wxString(a.s + (b ? b : "")); }
inline wxString operator+(const char *a, const wxString &b) { return wxString(std::string(a ? a : "") + b.s); }

// wxDirExists helper and minimal wxDir
inline bool wxDirExists(const wxString &p) { return std::filesystem::exists(std::filesystem::path(p.c_str())); }

enum { wxDIR_FILES = 1 };

enum wxDirTraverseResult { wxDIR_STOP = 0, wxDIR_CONTINUE = 1 };

class wxDirTraverser {
public:
  virtual ~wxDirTraverser() = default;
  virtual wxDirTraverseResult OnFile(const wxString &filename) { (void)filename; return wxDIR_CONTINUE; }
  virtual wxDirTraverseResult OnDir(const wxString &dirname) { (void)dirname; return wxDIR_CONTINUE; }
};

class wxDir {
public:
  wxDir(const wxString &p) : path_(p.c_str()), iter_(), end_() {
    if (std::filesystem::exists(path_)) iter_ = std::filesystem::directory_iterator(path_);
  }
  int GetAllFiles(const wxString &path, class wxArrayString *out, const wxString &pattern, int flags) {
    (void)path; (void)pattern; (void)flags; if (!out) return 0; return 0;
  }
  bool IsOpened() const { return std::filesystem::exists(path_); }
  bool GetFirst(wxString *filename) {
    if (!filename) return false;
    if (!std::filesystem::exists(path_)) return false;
    iter_ = std::filesystem::directory_iterator(path_);
    return GetNext(filename);
  }
  bool GetNext(wxString *filename) {
    if (!filename || iter_ == end_) return false;
    *filename = wxString(iter_->path().filename().u8string());
    ++iter_;
    return true;
  }
  wxDirTraverseResult Traverse(wxDirTraverser &trav, int flags = 0) {
    (void)flags;
    if (!std::filesystem::exists(path_)) return wxDIR_STOP;
    for (auto &entry : std::filesystem::directory_iterator(path_)) {
      if (entry.is_directory()) {
        if (trav.OnDir(wxString(entry.path().filename().u8string())) == wxDIR_STOP) return wxDIR_STOP;
      } else {
        if (trav.OnFile(wxString(entry.path().filename().u8string())) == wxDIR_STOP) return wxDIR_STOP;
      }
    }
    return wxDIR_CONTINUE;
  }
  static bool Exists(const wxString &p) { return wxDirExists(p); }
private:
  std::filesystem::path path_;
  std::filesystem::directory_iterator iter_;
  std::filesystem::directory_iterator end_;
};

// PROJECTION_UNKNOWN placeholder
enum { PROJECTION_UNKNOWN = 0 };
enum { PROJECTION_MERCATOR = 1 };

// Helper functions
template<typename T>
inline T wxMin(T a, T b) { return (a < b) ? a : b; }

template<typename T, typename U>
inline auto wxMin(T a, U b) -> decltype(a < b ? a : b) { return (a < b) ? a : b; }

template<typename T>
inline T wxMax(T a, T b) { return (a > b) ? a : b; }

template<typename T, typename U>
inline auto wxMax(T a, U b) -> decltype(a > b ? a : b) { return (a > b) ? a : b; }

// wx PATH flags
enum { wxPATH_MKDIR_FULL = 0, wxPATH_GET_VOLUME = 1, wxPATH_GET_SEPARATOR = 2 };

// Basic file IO stubs used by cm93.cpp when reading headers during porting
#include <fstream>

// Helper stub: simple spherical mercator-ish projection
inline double toSM(double lat, double lon, double clat, double clon, double* easting, double* northing) {
  (void)lat; (void)lon; (void)clat; (void)clon;
  *easting = lon * 111319.9;
  *northing = lat * 110574.0;
  return 0.0;
}

// Simple inverse of toSM for porting purposes
inline void fromSM(double easting, double northing, double clat, double clon, double *plat, double *plon) {
  (void)clat; (void)clon;
  *plon = easting / 111319.9;
  *plat = northing / 110574.0;
}

// Mercator and ellipsoid constants used in ported code
inline const double mercator_k0 = 1.0;
inline const double WGS84_semimajor_axis_meters = 6378137.0;

// Chart type constants stub
enum { CHART_TYPE_CM93 = 100 };
enum { CHART_TYPE_CM93COMP = 101 };

// Chart initialization types and flags
enum InitReturn { INIT_OK = 0, INIT_FAIL_REMOVE = 1, INIT_FAIL_NOFILE = 2, INIT_FAIL_NOERROR = 3 };
enum ChartInitFlag { FULL_INIT = 0, HEADER_ONLY = 1 };

// GetNativeScale stubs returning an integral scale id
inline int GetNativeScale(const ViewPort &vp) { (void)vp; return 0; }
inline int GetNativeScale() { return 0; }

// Cache validity placeholder
inline bool IsCacheValid() { return true; }

// Global stubs for cm93 rendering
inline bool g_bDebugCM93 = false;

// Viewport transformation stub
struct vp_transform { int x = 0; int y = 0; };

// Simple extent struct used by cm93chart::m_FullExtent
struct FullExtent_t { double ELON = 0.0; double WLON = 0.0; double NLAT = 0.0; double SLAT = 0.0; };

// Platform abstraction stubs  
class AbstractPlatform {
public:
  static void ShowBusySpinner() {}
  static void HideBusySpinner() {}
};

// Helper functions and constants
inline int wxRound(double x) { return static_cast<int>(std::round(x)); }

#ifndef PI
#define PI 3.14159265358979323846
#endif
#ifndef DEGREE
#define DEGREE (PI / 180.0)
#endif
#ifndef wxNOT_FOUND
#define wxNOT_FOUND -1
#endif

#define wxASSERT(x) ((void)0)

// Encoding stub
struct wxConvUTF8 {};
inline wxConvUTF8 wxConvUTF8;

// LUP and rendering type stubs
// Actual LUP and S57 types are defined in local s52s57.h stub.
enum { GEO_POINT = 0, GEO_LINE = 1, GEO_AREA = 2, GEO_META = 3, GEO_PRIM = 4, THUMB_ONLY = 2 };
enum { PI_PRIO_NUM = 16, PI_LUPNAME_NUM = 16, MASK_ALL = 0xFFFF };
#define WXUNUSED(x) x

// Global app/frame stubs
class TopFrame {
public:
  double GetBestVPScale(void * /*chart*/) { return 1.0; }
  void InvalidateAllGL() {}
};

namespace top_frame {
  inline TopFrame *Get() {
    static TopFrame instance;
    return &instance;
  }
};

// Vector association helper (stub)
inline void SendVectorChartObjectInfo(const wxString &cellname, const wxString &fe_name, const wxString &objname, double lat, double lon, double scale, int nativescale) {
  (void)cellname;
  (void)fe_name;
  (void)objname;
  (void)lat;
  (void)lon;
  (void)scale;
  (void)nativescale;
}

// S57 rendering functions
inline void GetAssociatedObjects() {}

// S57 chart processing (no-op stubs)
inline void ForceEdgePriorityEvaluate() {}
inline void AssembleLineGeometry() {}
inline void ClearDepthContourArray() {}
inline void BuildDepthContourArray() {}

// Viewport transform stub
struct vp_transform_t {
  double easting_vp_center = 0.0;
  double northing_vp_center = 0.0;
};
inline vp_transform_t vp_transform;

// Array_Of_M_COVR_Desc stub (simple vector wrapper)
template<typename T>
class WxListNode;

template<typename T>
class WxArray {
public:
  void Add(const T& v) { items.push_back(v); }
  void Add(T* v) { if (v) items.push_back(*v); }  // Allow pointer addition
  void Append(const T& v) { Add(v); }
  void Append(T* v) { Add(v); }
  size_t GetCount() const { return items.size(); }
  size_t size() const { return items.size(); }
  T& Item(size_t i) { return items[i]; }
  const T& Item(size_t i) const { return items[i]; }
  T& operator[](size_t i) { return items[i]; }
  const T& operator[](size_t i) const { return items[i]; }
  void Clear() { items.clear(); }
  std::vector<T> items;

  // Minimal linked-list node chain creation for compatibility with
  // legacy code that expects GetFirst()/GetNext() list traversal.
  WxListNode<T>* GetFirst() {
    if (items.empty()) return nullptr;
    WxListNode<T>* head = new WxListNode<T>();
    head->m_data = &items[0];
    WxListNode<T>* cur = head;
    for (size_t i = 1; i < items.size(); ++i) {
      WxListNode<T>* n = new WxListNode<T>();
      n->m_data = &items[i];
      cur->m_next = n;
      cur = n;
    }
    cur->m_next = nullptr;
    return head;
  }
};

template<typename T>
class WxListNode {
public:
  T* m_data = nullptr;
  WxListNode<T>* m_next = nullptr;
  T* GetData() { return m_data; }
  WxListNode<T>* GetNext() { return m_next; }
};

// Forward declare M_COVR_Desc for Array typedef (use class, not struct)
class M_COVR_Desc;

// Typedef for array container
using Array_Of_M_COVR_Desc = WxArray<M_COVR_Desc>;
using Array_Of_M_COVR_Desc_Ptr = WxArray<M_COVR_Desc*>;
using List_Of_M_COVR_Desc = WxArray<M_COVR_Desc>;  // List is same as Array for simplicity

// Convenience node typedef expected by cm93 code
using wxList_Of_M_COVR_DescNode = WxListNode<M_COVR_Desc>;
