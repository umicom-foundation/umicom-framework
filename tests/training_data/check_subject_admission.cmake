#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_data/check_subject_admission.cmake
#
# PURPOSE:
#   Verify wrappers fail before fixture writes when their compiled child is unavailable.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.25)
if(NOT DEFINED WRAPPER OR NOT DEFINED WORK OR NOT DEFINED CASE)
    message(FATAL_ERROR "Provide WRAPPER, WORK and CASE")
endif()
string(RANDOM LENGTH 20 ALPHABET 0123456789abcdef suffix)
set(root "${WORK}/admission-${suffix}")
file(MAKE_DIRECTORY "${root}")
set(subject "${root}/unbuilt-subject.exe")
if(CASE STREQUAL "directory")
    file(MAKE_DIRECTORY "${subject}")
elseif(NOT CASE STREQUAL "missing")
    message(FATAL_ERROR "Unknown admission scenario")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" "-DSUBJECT=${subject}" "-DWORK=${root}/corpus-output"
    -DCASE=valid -P "${WRAPPER}" RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 20)
if("${result}" STREQUAL "0" OR NOT error MATCHES "Training test subject is missing or not a file")
    message(FATAL_ERROR "Missing child subject was not diagnosed before execution: ${result}\n${output}\n${error}")
endif()
if(EXISTS "${root}/corpus-output")
    message(FATAL_ERROR "Wrapper wrote corpus fixtures before subject admission")
endif()
if(CASE STREQUAL "directory" AND NOT IS_DIRECTORY "${subject}")
    message(FATAL_ERROR "Wrapper changed the refused subject directory")
endif()
if(CASE STREQUAL "missing" AND EXISTS "${subject}")
    message(FATAL_ERROR "Wrapper created or fetched a missing subject")
endif()
