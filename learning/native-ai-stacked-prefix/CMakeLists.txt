#-----------------------------------------------------------------------------
# Umicom Framework | native stacked prefix semantics laboratory
# Validate right-padded causal reference before introducing a retained KV cache.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeStackedPrefix LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomNativeStackedPrefixInference.cmake")
