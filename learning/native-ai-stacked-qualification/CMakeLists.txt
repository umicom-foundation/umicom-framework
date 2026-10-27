#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-stacked-qualification/CMakeLists.txt
# PURPOSE: Isolated SDK demonstration of full-vector Transformer qualification.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeStackedQualification LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedQualification.cmake")
