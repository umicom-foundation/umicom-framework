#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-kv-session-pool/CMakeLists.txt
#
# PURPOSE:
#   Configure a dedicated native C23 cache-pool qualification laboratory.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeKvSessionPool LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedKvPool.cmake")
