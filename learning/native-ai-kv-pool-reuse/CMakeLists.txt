#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-kv-pool-reuse/CMakeLists.txt
#
# PURPOSE:
#   Compile the existing FP32 KV-store owner with opt-in idle reuse tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeKvPoolReuse LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedKvPool.cmake")
if(NOT TARGET umicom-native-kv-pool-reuse-demo)
    add_executable(umicom-native-kv-pool-reuse-demo
        "${CMAKE_CURRENT_LIST_DIR}/demo.c")
    target_link_libraries(umicom-native-kv-pool-reuse-demo PRIVATE
        Umicom::AiStackedKvPool)
    target_compile_features(umicom-native-kv-pool-reuse-demo PRIVATE c_std_23)
endif()
