#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-education-catalogue/CMakeLists.txt
# PURPOSE: Build native C23 educational HTML catalogue renderer and tests.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeEducationCatalogue LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomEducationCourseCatalogue.cmake")
