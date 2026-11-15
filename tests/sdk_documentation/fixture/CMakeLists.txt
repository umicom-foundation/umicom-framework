# Umicom Framework
# File: tests/sdk_documentation/fixture/CMakeLists.txt
#
# PURPOSE:
# Exercise the real documentation install helper with isolated public fixtures.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
project(UmicomSdkDocumentationFixture LANGUAGES NONE)
if(NOT DEFINED UMICOM_FRAMEWORK_SOURCE)
    message(FATAL_ERROR "Supply UMICOM_FRAMEWORK_SOURCE")
endif()
if(NOT DEFINED CMAKE_INSTALL_DATADIR)
    set(CMAKE_INSTALL_DATADIR share)
endif()
include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomSdkDocumentation.cmake")
umicom_install_sdk_documentation("${CMAKE_CURRENT_SOURCE_DIR}")
