# CM93 native integration (scaffold)

This folder contains a scaffold for a native `cm93_wrapper` shared library which will expose a
small C API to the Flutter app. The current implementation is a placeholder renderer that
produces a sea-like RGBA bitmap. The real integration should extract the CM93 rendering
components from OpenCPN and replace the placeholder `cm93_render` with a call into the
CM93 renderer.

Steps to integrate OpenCPN CM93 code:

1. Clone OpenCPN sources (matching a stable release):

   git clone https://github.com/OpenCPN/OpenCPN.git

2. Identify the CM93-related sources (files named `cm93*`, `cm93.cpp`, headers and dependencies).
   Copy the minimal set of files and headers into `native/cm93/external/` or create a submodule.

3. Modify `native/cm93/CMakeLists.txt` to include the external sources and link any required
   third-party libraries. You may need to pull in small utility code from OpenCPN (s57, s52lib)
   or stub parts of it.

4. Implement the rendering glue in `src/cm93_wrapper.c` to call into the extracted CM93 API and
   return an RGBA buffer.

5. Build the shared library with CMake for each target platform and place the resulting
   binary next to the Flutter app executable so `DynamicLibrary.open` can find it.

Notes:
- Licensing: OpenCPN is GPL/LGPL; confirm license compatibility before distributing derived
  binaries. If you only use the library locally, this may be acceptable, but for redistribution
  consult legal guidance.
- Toolchains: Windows requires MSVC (or Mingw), macOS uses clang, Linux uses gcc/clang.
