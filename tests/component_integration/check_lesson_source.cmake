#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/component_integration/check_lesson_source.cmake
#
# PURPOSE:
#   Keep the displayed current consumer example identical to its complete source.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED FRAMEWORK OR NOT IS_DIRECTORY "${FRAMEWORK}/include/umicom")
    message(FATAL_ERROR "FRAMEWORK must identify the Framework source")
endif()
file(READ "${FRAMEWORK}/examples/component_integration/main.c" _source)
file(READ "${FRAMEWORK}/docs/learning/build-integrated-components.html" _guide)
# Ignore checkout line-ending conversion, but preserve every source character.
# Historical listings remain outside this explicitly identified current block.
string(REPLACE "\r\n" "\n" _source "${_source}")
string(REPLACE "\r\n" "\n" _guide "${_guide}")
string(REPLACE "&" "&amp;" _source "${_source}")
string(REPLACE "<" "&lt;" _source "${_source}")
string(REPLACE ">" "&gt;" _source "${_source}")
string(REPLACE "\"" "&quot;" _source "${_source}")
set(_marker "<code class=\"c\" data-source=\"examples/component_integration/main.c\">")
string(FIND "${_guide}" "${_marker}${_source}</code>" _found)
if(_found EQUAL -1)
    message(FATAL_ERROR "The current learning-guide example differs from examples/component_integration/main.c")
endif()
string(LENGTH "${_marker}" _marker_length)
math(EXPR _after "${_found} + ${_marker_length}")
string(SUBSTRING "${_guide}" ${_after} -1 _remaining)
string(FIND "${_remaining}" "${_marker}" _duplicate)
if(NOT _duplicate EQUAL -1)
    message(FATAL_ERROR "The learning guide declares more than one current consumer source block")
endif()
message(STATUS "Current consumer source and displayed example agree; compilation and execution are separate checks")
