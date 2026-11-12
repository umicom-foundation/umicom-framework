#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/toolchain_lifecycle/check_user_presets.cmake
#
# PURPOSE:
#   Qualify JSON parsing and literal CMake macro interpretation without compiling an application.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
if(CMAKE_VERSION VERSION_LESS 3.25)
    message("SKIP: version-6 preset interpretation requires CMake 3.25 or newer")
    return()
endif()
if(NOT DEFINED TOOL OR NOT EXISTS "${TOOL}")
    message(FATAL_ERROR "Supply the authored preset fixture executable")
endif()
find_program(_ninja NAMES ninja)
if(NOT _ninja)
    message("SKIP: Ninja is unavailable for preset interpretation")
    return()
endif()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _token)
set(_root "${CMAKE_CURRENT_BINARY_DIR}/umicom-preset-interpretation-${_token}")
cmake_path(ABSOLUTE_PATH _root NORMALIZE OUTPUT_VARIABLE _root)
set(_owner "${CMAKE_CURRENT_BINARY_DIR}")
cmake_path(ABSOLUTE_PATH _owner NORMALIZE OUTPUT_VARIABLE _owner)
cmake_path(IS_PREFIX _owner "${_root}" NORMALIZE _inside)
if(NOT _inside OR _root STREQUAL _owner OR EXISTS "${_root}")
    message(FATAL_ERROR "Refuse an unowned preset fixture destination")
endif()
file(MAKE_DIRECTORY "${_root}")
execute_process(COMMAND "${TOOL}" cmake-fixture "${_ninja}"
    OUTPUT_FILE "${_root}/CMakeUserPresets.json" ERROR_VARIABLE _error RESULT_VARIABLE _result)
if(NOT _result EQUAL 0)
    message(FATAL_ERROR "Fixture serialization failed: ${_result} ${_error}; preserve ${_root}")
endif()
file(READ "${_root}/CMakeUserPresets.json" _json)
string(JSON _version GET "${_json}" version)
string(JSON _compiler GET "${_json}" configurePresets 0 cacheVariables CMAKE_C_COMPILER)
if(NOT _version EQUAL 6)
    message(FATAL_ERROR "Wrong preset schema")
endif()
set(_escaped [==[C:/training/${dollar}{sourceDir}/cc"quoted\tail]==])
if(NOT _compiler STREQUAL _escaped)
    message(FATAL_ERROR "JSON-decoded compiler differs from literal macro encoding")
endif()
file(WRITE "${_root}/CMakeLists.txt" [==[
cmake_minimum_required(VERSION 3.25)
project(umicom_preset_interpretation LANGUAGES NONE)
set(_expected_compiler [=[C:/training/${sourceDir}/cc"quoted\tail]=])
set(_expected_prefix [=[C:/training/$env{PATH};C:/literal/${dollar}]=])
if(NOT CMAKE_C_COMPILER STREQUAL _expected_compiler)
    message(FATAL_ERROR "CMake expanded a literal compiler-path macro")
endif()
if(NOT CMAKE_PREFIX_PATH STREQUAL _expected_prefix)
    message(FATAL_ERROR "CMake expanded a literal prefix-path macro")
endif()
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT BUILD_TESTING)
    message(FATAL_ERROR "Established Debug/testing options changed")
endif()
file(WRITE "${CMAKE_BINARY_DIR}/interpretation-complete.txt" "literal paths retained\n")
]==])
execute_process(COMMAND "${CMAKE_COMMAND}" --preset umicom-auto-debug
    WORKING_DIRECTORY "${_root}" RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(NOT _result EQUAL 0)
    message(FATAL_ERROR "Preset interpretation failed: ${_result}\n${_output}\n${_error}\nPreserve ${_root}")
endif()
if(NOT EXISTS "${_root}/build/umicom-auto-debug/interpretation-complete.txt")
    message(FATAL_ERROR "The intentional binaryDir macro was not used")
endif()
# Remove only this newly created direct child after successful interpretation.
# Failed fixtures remain available for diagnosis.
cmake_path(IS_PREFIX _owner "${_root}" NORMALIZE _inside)
if(NOT _inside OR _root STREQUAL _owner)
    message(FATAL_ERROR "Refuse cleanup outside the fixture owner")
endif()
file(REMOVE_RECURSE "${_root}")
