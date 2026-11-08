#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/android_profile_name/CMakeLists.txt
#
# PURPOSE:
#   Provide an isolated native-host build for JNI adapter contracts without a desktop application.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.26)
project(UmicomAndroidProfileContracts LANGUAGES C)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomProfileName.cmake")
if(BUILD_TESTING)
    set(UMICOM_BUILD_ANDROID_JNI_TESTS ON CACHE BOOL "Use actual JDK JNI declarations for host contracts")
    include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAndroidProfileNameTests.cmake")
endif()
