#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-course-platform/CMakeLists.txt
# PURPOSE: Independently test native HTML learning navigation and UTF-8 scalar
#          streaming without loading a model, UI toolkit or scripting runtime.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeAiCoursePlatform LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomEducationCourseCatalogue.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomUtf8Stream.cmake")
