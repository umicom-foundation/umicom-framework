#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-inference-memory-plan/CMakeLists.txt
# PURPOSE: Build a standalone example of the canonical native cache planner.
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomAiMemoryPlan LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiInferenceMemoryPlan.cmake")
