#ifndef CM93_WRAPPER_H
#define CM93_WRAPPER_H
#include <stdint.h>
#ifdef USE_OCN_STUBS
#include "ocpn_stubs.h"
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
#  if defined(CM93_WRAPPER_EXPORTS) || defined(cm93_wrapper_EXPORTS)
#    define CM93_API __declspec(dllexport)
#  else
#    define CM93_API __declspec(dllimport)
#  endif
#else
#  define CM93_API
#endif
#ifdef __cplusplus
extern "C" {
#endif

CM93_API int cm93_init(const char* cm93_dir);
CM93_API uint8_t* cm93_render(int width, int height, double lat, double lon, int zoom);
CM93_API void cm93_free_buffer(uint8_t* buf);
CM93_API int cm93_version();

#ifdef __cplusplus
}
#endif
#endif
