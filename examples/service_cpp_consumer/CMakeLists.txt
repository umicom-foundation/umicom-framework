#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/service_cpp_consumer/CMakeLists.txt
#
# PURPOSE:
#   Consume selected installed C service SDK components from independent C++ translation units.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.25)
project(UmicomServiceCppConsumer LANGUAGES C CXX)
set(UMICOM_CPP_SERVICE_COMPONENTS
    "codeguard;data;desktop;developer;finance;frontend;integration;observability;security"
    CACHE STRING "Exported service components selected for C++ qualification")
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ${UMICOM_CPP_SERVICE_COMPONENTS})
include(CTest)
if(BUILD_TESTING)
    # Qualification sources are shared; implementation and include dependencies
    # come from imported SDK targets, with no source-tree include injection.
    include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/UmicomServiceCppConsumer.cmake")
    umicom_add_service_cpp_checks(umicom-service-sdk-cpp "" ${UMICOM_CPP_SERVICE_COMPONENTS})
endif()
