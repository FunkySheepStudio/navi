
cmake_minimum_required(VERSION 3.10)
project(cm93_wrapper C CXX)

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(EXTERNAL_CM93_CPP ${EXTERNAL_DIR}/cm93.cpp)
set(EXTERNAL_CM93_H   ${EXTERNAL_DIR}/cm93.h)

set(SOURCES src/cm93_wrapper.cpp)
if(EXISTS ${EXTERNAL_CM93_CPP})
  message(STATUS "Found external cm93.cpp, will include in build for porting")
  list(APPEND SOURCES ${EXTERNAL_CM93_CPP})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/external)
  add_definitions(-DUSE_OCN_STUBS)
else()
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

add_library(cm93_wrapper SHARED ${SOURCES})
set_target_properties(cm93_wrapper PROPERTIES OUTPUT_NAME "cm93_wrapper")

# Export C API symbols when building the DLL on Windows
if(MSVC)
  set_target_properties(cm93_wrapper PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/cm93_wrapper.def")
endif()
target_compile_definitions(cm93_wrapper PRIVATE CM93_WRAPPER_EXPORTS)

target_compile_features(cm93_wrapper PRIVATE cxx_std_17)

install(TARGETS cm93_wrapper
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
