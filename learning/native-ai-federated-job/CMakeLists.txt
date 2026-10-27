#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-federated-job/CMakeLists.txt
#
# PURPOSE:
#   Build the original Framework federated-round and new admission layer
#   together as a standalone native C23 experiment with its real unit tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeFederatedJob LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiFederatedJob.cmake")
