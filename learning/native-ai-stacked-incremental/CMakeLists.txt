#-----------------------------------------------------------------------------
# Umicom Framework | Native stacked incremental inference
# PURPOSE: Test one-token tape composition against the unchanged full-context
#          stacked decoder and explicit eviction fallback.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeStackedIncremental LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedIncremental.cmake")
