#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-federated-wire/CMakeLists.txt
#
# PURPOSE:
#   Build the native C23 bounded federated wire codec and deterministic
#   multiprocess teaching example against existing Framework source.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeFederatedWire LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiFederatedWire.cmake")
