#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/compiler_cpp_consumer/CMakeLists.txt
#
# PURPOSE:
#   Qualify an installed C compiler SDK from independent C++ consumer translation units.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomCompilerCppConsumer LANGUAGES C CXX)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS compiler)
include(CTest)
if(BUILD_TESTING)
    # The shared qualification sources call exported APIs only. This example
    # does not compile Framework implementation files or use source-tree headers.
    include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/UmicomCompilerCppConsumer.cmake")
    umicom_add_compiler_cpp_checks(umicom-compiler-sdk-cpp "" Umicom::compiler)
    if(WIN32)
        enable_language(RC)
    endif()
    umicom_apply_application_branding(TARGET umicom-compiler-sdk-cpp-workflow
        PRODUCT_NAME "Umicom Compiler C++ SDK Consumer"
        INTERNAL_NAME umicom-compiler-sdk-cpp-workflow NO_RUNTIME_DEPLOYMENT)
endif()
