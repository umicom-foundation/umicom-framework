#-----------------------------------------------------------------------------
# Umicom Framework: Frozen Decoder Projection Adaptation Laboratory
# PURPOSE: Build and test real Transformer projection adapters using the
#          existing native decoder and reviewed assistant-response loss.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeDecoderProjectionAdaptation LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiDecoderProjectionAdaptation.cmake")
