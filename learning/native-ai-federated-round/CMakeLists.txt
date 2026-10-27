#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-federated-round/CMakeLists.txt
#
# PURPOSE:
#   Provide a standalone C23 CMake/CTest entry point for the weighted
#   federated learning laboratory.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeFederatedRound LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiFederatedRound.cmake")
