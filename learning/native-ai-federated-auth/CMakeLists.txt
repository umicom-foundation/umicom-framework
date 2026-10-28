#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-federated-auth/CMakeLists.txt
#
# PURPOSE:
#   Build opt-in native authenticated federated frame and checksum regression
#   experiments using the existing canonical Framework SHA-256 source.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeFederatedAuth LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiFederatedAuthenticatedFrame.cmake")
