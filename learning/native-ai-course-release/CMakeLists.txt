# Umicom native C23 course release checker; Python site tools remain optional.
# Sammy Hegab / Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeCourseRelease LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomEducationCourseRelease.cmake")
