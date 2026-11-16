# Umicom Framework
# File: tests/sdk_composition/check_composition.cmake
#
# PURPOSE:
#   Check source-free SDK composition and refusal contracts through isolated CMake configuration fixtures.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
if(NOT IS_ABSOLUTE "${FRAMEWORK}" OR NOT IS_DIRECTORY "${FRAMEWORK}" OR
   NOT IS_ABSOLUTE "${WORK}" OR NOT DEFINED CASE)
    message(FATAL_ERROR "Supply absolute FRAMEWORK, WORK and a composition CASE")
endif()
set(_cases imported source missing-example unknown-profile duplicate missing-component)
if(NOT CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown composition case")
endif()
file(MAKE_DIRECTORY "${WORK}/source" "${WORK}/sdk/cmake" "${WORK}/sdk/examples")
configure_file("${FRAMEWORK}/tests/sdk_composition/consumer/CMakeLists.txt"
    "${WORK}/source/CMakeLists.txt" COPYONLY)
# Copy only installed consumer helpers, never Framework implementation sources.
foreach(_helper ApplicationComposition CompilePolicy Sanitizers ThinApplicationRuntime ApplicationProductisationContribution CreativeApplication)
    configure_file("${FRAMEWORK}/cmake/Umicom${_helper}.cmake"
        "${WORK}/sdk/cmake/Umicom${_helper}.cmake" COPYONLY)
endforeach()
configure_file("${FRAMEWORK}/examples/creative_workspace_main.c"
    "${WORK}/sdk/examples/creative_workspace_main.c" COPYONLY)
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${WORK}/source" -B "${WORK}/build"
    "-DCASE=${CASE}" "-DFRAMEWORK=${FRAMEWORK}" "-DSDK_FIXTURE=${WORK}/sdk"
    "-DCMAKE_C_COMPILER=${COMPILER}" "-DCMAKE_MAKE_PROGRAM=${MAKE_PROGRAM}" -G "${GENERATOR}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(CASE STREQUAL "imported" OR CASE STREQUAL "source")
    if(NOT _result STREQUAL "0" OR NOT _output MATCHES "Composition contract accepted")
        message(FATAL_ERROR "Expected composition configuration: ${_output}\n${_error}")
    endif()
else()
    if(CASE STREQUAL "missing-example")
        set(_expected "Creative application example source is missing")
    elseif(CASE STREQUAL "unknown-profile")
        set(_expected "Unknown creative profile")
    elseif(CASE STREQUAL "duplicate")
        set(_expected "Creative workspace already attached")
    else()
        set(_expected "requires the Framework creative_workspace component")
    endif()
    if(_result STREQUAL "0" OR NOT _error MATCHES "${_expected}")
        message(FATAL_ERROR "Expected composition refusal: ${_output}\n${_error}")
    endif()
endif()
