cmake_minimum_required(VERSION 3.13)
enable_testing()
project(latticeZK LANGUAGES C CXX VERSION 0.1)
#find_package(MKL CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
  "-fPIC"
  "-Wall" "-Wpedantic" "-Wextra" "-fexceptions"
  "$<$<CONFIG:RELEASE>:-Ofast>"
  "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb;--coverage>"
)
add_link_options(
  "$<$<CONFIG:DEBUG>:--coverage>"
)

add_subdirectory(src)
add_subdirectory(test)
