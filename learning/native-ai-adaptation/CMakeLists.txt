#-----------------------------------------------------------------------------
# Umicom Framework: Native AI adaptation laboratory
# PURPOSE: Build C23 low-rank fine-tuning primitives and explicit training
#          source admission against the canonical Framework training tape.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeAiAdaptation LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiAdaptation.cmake")
