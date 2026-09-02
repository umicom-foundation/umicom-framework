#-----------------------------------------------------------------------------
# Umicom Framework
# File: templates/compiler/cpp/CMakeLists.txt
#
# PURPOSE:
#   Provide reusable cpp source for projects created with Umicom tooling.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

# Umicom C++ application template | Sammy Hegab | Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomCppTemplate LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(umicom-cpp src/main.cpp)
