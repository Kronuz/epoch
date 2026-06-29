cmake_minimum_required(VERSION 3.14)
project(epoch LANGUAGES CXX)

# --- Library target ---
# epoch is header-only and dependency-free: one header (epoch.hh) with a single
# templated `epoch::now<Period>()` that reads the system clock and returns the
# time since the epoch as an integer count in the requested duration (seconds by
# default). Nothing but <chrono> behind it. So the target is a pure INTERFACE
# library that just puts the source dir on the consumer's include path.
add_library(epoch INTERFACE)
target_include_directories(epoch INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(epoch INTERFACE cxx_std_20)
# Alias so consumers can link `epoch::epoch` whether they add this via
# add_subdirectory or FetchContent.
add_library(epoch::epoch ALIAS epoch)

# Only build the test when this repo is the top-level project, so consumers
# (e.g. via FetchContent) don't build our tests.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  enable_testing()
  add_executable(epoch_test test/test.cc)
  target_link_libraries(epoch_test PRIVATE epoch)
  add_test(NAME epoch COMMAND epoch_test)
endif()
