// ocpn_port_shim.h
// Minimal shim to help compile extracted OpenCPN CM93 sources during porting.

#pragma once
#include "ocpn_stubs.h"

// Macros are now defined in ocpn_stubs.h; don't redefine here
#ifndef WX_DECLARE_OBJARRAY
#define WX_DECLARE_OBJARRAY(clazz, name)
#endif
#ifndef WX_DEFINE_OBJARRAY
#define WX_DEFINE_OBJARRAY(name)
#endif
#ifndef WX_DECLARE_LIST
#define WX_DECLARE_LIST(name, listname)
#endif
#ifndef WX_DEFINE_LIST
#define WX_DEFINE_LIST(name)
#endif

// Minimal wx types
typedef char wxChar;

// File streams are now defined in ocpn_stubs.h; do not redefine

// Minimal wxFileName stub
class wxFileName {
public:
  wxFileName() = default;
  wxFileName(const std::string& p) : path(p) {}
  wxFileName(const wxString& p) : path(p.c_str()) {}
  bool DirExists() const { return std::filesystem::is_directory(std::filesystem::path(path)); }
  std::string GetPath() const { return std::filesystem::path(path).parent_path().string(); }
  std::string GetPath(int flags) const { (void)flags; return GetPath(); }
  wxString GetFullName() const { return wxString(std::filesystem::path(path).filename().string()); }
  wxString GetName() const { return wxString(std::filesystem::path(path).stem().string()); }
  wxString GetExt() const { std::string ext = std::filesystem::path(path).extension().string(); if (!ext.empty() && ext[0] == '.') ext.erase(0, 1); return wxString(ext); }
  void Assign(const wxString &p) { path = p.c_str(); }
  static void Mkdir(const std::string& p, int, int) { std::filesystem::create_directories(std::filesystem::path(p)); }
  static wxChar GetPathSeparator() { return '/'; }
  static bool FileExists(const std::string &p) { return std::filesystem::exists(std::filesystem::path(p)); }
  static bool FileExists() { return false; }
  static wxString CreateTempFileName(const wxString &name) {
    std::string p = name.c_str();
    std::filesystem::path tmp(p);
    tmp += ".tmp";
    return wxString(tmp.string());
  }
  // Remove last directory component from the stored path
  void RemoveLastDir() { std::filesystem::path p(path); path = p.parent_path().string(); }
private:
  std::string path;
};

// Minimal array/string stubs
class wxArrayString {
public:
  void Add(const std::string &s) { v.push_back(s); }
  // Add with count: mimic wxArrayString::Add(str, count)
  void Add(const std::string &s, size_t count) { v.assign(count, s); }
  void Insert(size_t idx, const std::string &s) { if(idx<=v.size()) v.insert(v.begin()+idx, s); }
  // Insert wxString, index
  void Insert(const wxString &s, size_t idx) { std::string tmp(s.c_str()); Insert(idx, tmp); }
  void RemoveAt(size_t idx) { if(idx<v.size()) v.erase(v.begin()+idx); }
  size_t GetCount() const { return v.size(); }
  const std::string& Item(size_t i) const { return v[i]; }
  std::string& operator[](size_t i) { return v[i]; }
  int Index(const wxString &s) const { std::string t = s.c_str(); for (size_t i=0;i<v.size();++i) if (v[i]==t) return static_cast<int>(i); return wxNOT_FOUND; }
private:
  std::vector<std::string> v;
};

// Global helper used in ported code
inline bool wxFileExists(const wxString &p) { return wxFileName::FileExists(p.c_str()); }

// Minimal logging utility used in cm93.cpp
inline void LogMessageOnce(const char* msg) { (void)msg; }
inline void LogMessageOnce(const wxString& msg) { (void)msg; }

// Provide simple PI constant
#ifndef PI
#define PI 3.14159265358979323846
#endif
