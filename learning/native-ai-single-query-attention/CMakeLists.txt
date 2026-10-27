#-----------------------------------------------------------------------------
# Umicom Framework | Standalone single-query attention qualification
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeSingleQueryAttention LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
option(UMICOM_SINGLE_QUERY_WITH_CANONICAL_TAPE
    "Build a second test against the existing full Framework attention tape" OFF)
if(UMICOM_SINGLE_QUERY_WITH_CANONICAL_TAPE)
    include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedIncremental.cmake")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiSingleQueryAttention.cmake")
