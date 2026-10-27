#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-inference-parity/CMakeLists.txt
# PURPOSE: Standalone native C23 numerical comparison regression and SDK lab.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeInferenceParity LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiInferenceParity.cmake")
