#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/check_console_startup_failure.cmake
#
# PURPOSE:
#   Verify incomplete console runtime fixtures fail promptly with stderr output.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT EXISTS "${EXE}" OR NOT WORK)
    message(FATAL_ERROR "EXE and a private WORK directory are required")
endif()
# Every case owns an isolated copy; the successful startup fixture stays intact.
set(_root "${WORK}/${CASE}")
file(MAKE_DIRECTORY "${_root}/bin")
file(COPY_FILE "${EXE}" "${_root}/bin/startup.exe" ONLY_IF_DIFFERENT)
if(CASE STREQUAL "missing-marker")
    set(_expected "runtime has not been deployed")
elseif(CASE STREQUAL "invalid-media")
    file(MAKE_DIRECTORY "${_root}/share/umicom/runtime" "${_root}/lib")
    file(WRITE "${_root}/share/umicom/runtime/deployment.marker" "fixture\n")
    file(WRITE "${_root}/lib/gstreamer-1.0" "not a directory\n")
    set(_expected "invalid plugin layout")
elseif(CASE STREQUAL "invalid-cache")
    file(MAKE_DIRECTORY "${_root}/share/umicom/runtime")
    file(WRITE "${_root}/share/umicom/runtime/deployment.marker" "fixture\n")
    # A directory at the cache filename cannot be opened as cache text.
    file(MAKE_DIRECTORY "${_root}/share/umicom/runtime/gdk-pixbuf-loaders.cache.in")
    set(_expected "image-loader cache could not be created")
else()
    message(FATAL_ERROR "Unknown case: ${CASE}")
endif()
execute_process(COMMAND "${_root}/bin/startup.exe"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 5)
if(NOT _result STREQUAL "1" OR NOT _stderr MATCHES "${_expected}")
    message(FATAL_ERROR "Console bootstrap did not fail promptly: ${_result}\n${_stdout}\n${_stderr}")
endif()
message(STATUS "Console bootstrap refused ${CASE} without an interactive dialog")
