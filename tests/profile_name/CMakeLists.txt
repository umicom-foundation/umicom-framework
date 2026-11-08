#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/profile_name/CMakeLists.txt
#
# PURPOSE:
#   Build the portable C contract independently of the full SDK and optional C++ clients.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)
project(UmicomProfileNameContract LANGUAGES C)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomProfileName.cmake")
if(BUILD_TESTING)
    include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomProfileNameTests.cmake")
endif()
