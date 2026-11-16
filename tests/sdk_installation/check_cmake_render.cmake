# Umicom Framework
# File: tests/sdk_installation/check_cmake_render.cmake
#
# PURPOSE:
# Parse emitted native SDK consumer text with controlled CMake command observers.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED EXECUTABLE OR NOT IS_ABSOLUTE "${EXECUTABLE}" OR
   NOT DEFINED TEST_BINARY OR NOT IS_ABSOLUTE "${TEST_BINARY}")
    message(FATAL_ERROR "Supply absolute EXECUTABLE and TEST_BINARY paths")
endif()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _id)
set(_root "${TEST_BINARY}/cmake-render-${_id}")
if(EXISTS "${_root}")
    message(FATAL_ERROR "CMake render fixture directory already exists")
endif()
file(MAKE_DIRECTORY "${_root}")
execute_process(COMMAND "${EXECUTABLE}" emit
    RESULT_VARIABLE _emitted OUTPUT_VARIABLE _text ERROR_VARIABLE _error TIMEOUT 20)
file(WRITE "${_root}/emit.log" "${_error}")
if(NOT _emitted STREQUAL "0")
    message(FATAL_ERROR "Native renderer failed: ${_emitted}: ${_error}")
endif()
file(WRITE "${_root}/generated.cmake" "${_text}")
# Observer functions inspect the real parser's argument boundaries. They neither
# discover a package nor create a link target or invoke a compiler.
file(WRITE "${_root}/observe.cmake" [=[
cmake_minimum_required(VERSION 3.24)
set(PROJECT_NAME ConsumerFixture)
set(_package_seen FALSE)
set(_link_seen FALSE)
function(find_package)
    if(NOT ARGC EQUAL 4 OR NOT ARGV0 STREQUAL "UmicomFramework" OR
       NOT ARGV1 STREQUAL "0.9" OR NOT ARGV2 STREQUAL "CONFIG" OR
       NOT ARGV3 STREQUAL "REQUIRED")
        message(FATAL_ERROR "Unexpected package arguments: ${ARGV}")
    endif()
    set(_package_seen TRUE PARENT_SCOPE)
endfunction()
function(target_link_libraries)
    if(NOT ARGC EQUAL 4 OR NOT ARGV0 STREQUAL "ConsumerFixture" OR
       NOT ARGV1 STREQUAL "PRIVATE" OR NOT ARGV2 STREQUAL "Umicom::sdk" OR
       NOT ARGV3 STREQUAL "Umicom::delivery")
        message(FATAL_ERROR "Unexpected target arguments: ${ARGV}")
    endif()
    set(_link_seen TRUE PARENT_SCOPE)
endfunction()
include("${CMAKE_CURRENT_LIST_DIR}/generated.cmake")
if(NOT _package_seen OR NOT _link_seen)
    message(FATAL_ERROR "Generated commands did not reach both observers")
endif()
]=])
execute_process(COMMAND "${CMAKE_COMMAND}" -P "${_root}/observe.cmake"
    RESULT_VARIABLE _parsed OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 20)
file(WRITE "${_root}/parse.log" "${_out}\n${_err}")
if(NOT _parsed STREQUAL "0")
    message(FATAL_ERROR "Generated CMake parsing failed: ${_out}\n${_err}")
endif()
