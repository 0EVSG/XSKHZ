cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)

project(XSKHZ VERSION 0.1.0)

# Doxygen documentation.
include(Documentation.cmake)

# Build settings.
include(Build.cmake)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BUILD_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BUILD_COMPILER_FLAGS}")


## ViFi Executable ##
# ViFi intermediate library and dependencies.
set(VIFI_SRC
  ViFi/FileTree.cpp
  ViFi/FileOpRunner.cpp
  ViFi/FileOpSequence.cpp
  ViFi/WriteText.cpp
  ViFi/ReadText.cpp
  ViFi/ScanDirectory.cpp
)

set(VIFI_HDR
  ViFi/FileTree.hpp
  ViFi/FileOpRunner.hpp
  ViFi/FileOpSequence.hpp
  ViFi/WriteText.hpp
  ViFi/ReadText.hpp
  ViFi/ScanDirectory.hpp
)

add_library(ViFiLib ${VIFI_SRC} ${VIFI_HDR})
target_link_libraries(ViFiLib
  PUBLIC c++experimental
)
target_include_directories(ViFiLib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# ViFi executable target.
file(READ COPYRIGHT.md COPYRIGHT_TEXT)
configure_file(ViFi/Copyright.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/Copyright.hpp)

set(VIFI_MAIN
  ${CMAKE_CURRENT_BINARY_DIR}/Copyright.hpp
  ViFi/main.cpp
)

add_executable(ViFi ${VIFI_MAIN})
target_link_libraries(ViFi PRIVATE ViFiLib)
target_include_directories(ViFi PRIVATE ViFiLib ${CMAKE_CURRENT_BINARY_DIR})

configure_file(ViFi/vifi.in ${CMAKE_CURRENT_BINARY_DIR}/vifi)

## Installation ##
# Install ViFi executable.
install(TARGETS ViFi EXPORT ExportTargets
  RUNTIME DESTINATION bin
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/vifi
  DESTINATION bin
)


## Tests (optional) ##
# Build option, defaults to on if Google C++ Testing Framework is found.
find_package(GTest)
option(BUILD_TESTS
  "Build tests (requires Google C++ Testing Framework)."
  ${GTEST_FOUND}
)

# Build target for tests.
if (BUILD_TESTS)
  if (NOT GTEST_FOUND)
    message(FATAL_ERROR "Google C++ Testing Framework is required for tests.")
  endif (NOT GTEST_FOUND)

  set(TEST_SRC
    Tests/FileTreeMatch.cpp
    Tests/TextAndBackAgain.cpp
  )
  add_executable(ViFiTests ${TEST_SRC})
  target_link_libraries(ViFiTests PRIVATE ViFiLib GTest::GTest GTest::Main)
  target_include_directories(ViFiTests PRIVATE ViFiLib)
endif (BUILD_TESTS)
