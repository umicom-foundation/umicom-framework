#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/test_gstreamer_resources.cmake
#
# PURPOSE:
#   Qualify media payload selection without loading or executing media plugins.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(_name FRAMEWORK WORK CASE)
    if(NOT DEFINED ${_name} OR "${${_name}}" STREQUAL "")
        message(FATAL_ERROR "Missing fixture argument: ${_name}")
    endif()
endforeach()
if(CASE MATCHES "^(missing-scanner|directory-scanner|linked-plugin|file-plugin-root)$" AND NOT PROBE)
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DFRAMEWORK=${FRAMEWORK}"
        "-DWORK=${WORK}" "-DCASE=${CASE}" -DPROBE=ON -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
    if(_result STREQUAL "0" OR NOT _error MATCHES "GStreamer")
        message(FATAL_ERROR "Expected an identified media-layout refusal: ${_output}\n${_error}")
    endif()
    return()
endif()
include("${FRAMEWORK}/cmake/windows/GStreamerResources.cmake")
# Unique fixture directories preserve earlier evidence and avoid deleting work.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _nonce)
set(_prefix "${WORK}/${_nonce}/selected runtime")
if(CASE STREQUAL "file-plugin-root")
    file(MAKE_DIRECTORY "${_prefix}/bin" "${_prefix}/lib")
    file(WRITE "${_prefix}/bin/libgstreamer-1.0-0.dll" "selection fixture")
    file(WRITE "${_prefix}/lib/gstreamer-1.0" "a file cannot contain plugins")
    umicom_windows_gstreamer_resources("${_prefix}" _actual _scanner)
    message(FATAL_ERROR "Unexpected media root acceptance")
endif()
file(MAKE_DIRECTORY "${_prefix}/bin" "${_prefix}/lib/gstreamer-1.0")
set(_plugins "${_prefix}/lib/gstreamer-1.0")
if(CASE STREQUAL "no-runtime")
    file(WRITE "${_plugins}/orphan.dll" "not selected without the runtime")
    umicom_windows_gstreamer_resources("${_prefix}" _actual _scanner)
    if(_actual OR _scanner)
        message(FATAL_ERROR "Orphan media files became a runtime")
    endif()
    return()
endif()
file(WRITE "${_prefix}/bin/libgstreamer-1.0-0.dll" "selection fixture; not a PE image")
if(CASE STREQUAL "no-plugins")
    umicom_windows_gstreamer_resources("${_prefix}" _actual _scanner)
    if(_actual OR _scanner)
        message(FATAL_ERROR "Media-free runtime gained invented payload")
    endif()
    return()
endif()
file(WRITE "${_plugins}/libgstplayback.dll" "selection fixture")
file(WRITE "${_plugins}/libgstapp.dll" "selection fixture")
set(_helper "${_prefix}/libexec/gstreamer-1.0/gst-plugin-scanner.exe")
file(MAKE_DIRECTORY "${_prefix}/libexec/gstreamer-1.0")
if(CASE STREQUAL "directory-scanner")
    file(MAKE_DIRECTORY "${_helper}")
elseif(NOT CASE STREQUAL "missing-scanner")
    file(WRITE "${_helper}" "selection fixture; never executed")
endif()
if(CASE STREQUAL "linked-plugin")
    file(WRITE "${WORK}/${_nonce}/outside.dll" "outside prefix")
    file(CREATE_LINK "${WORK}/${_nonce}/outside.dll" "${_plugins}/linked.dll"
        SYMBOLIC RESULT _linked)
    if(NOT _linked STREQUAL "0")
        message(FATAL_ERROR "Fixture could not create the required symbolic link: ${_linked}")
    endif()
elseif(CASE STREQUAL "ignore-registry")
    file(WRITE "${_plugins}/registry.bin" "machine-specific cache")
    file(WRITE "${_plugins}/notes.txt" "not a plugin")
    file(MAKE_DIRECTORY "${_plugins}/nested")
    file(WRITE "${_plugins}/nested/unrelated.dll" "not a direct plugin")
elseif(NOT CASE MATCHES "^(plugins|missing-scanner|directory-scanner)$")
    message(FATAL_ERROR "Unknown media selection scenario: ${CASE}")
endif()
umicom_windows_gstreamer_resources("${_prefix}" _actual _scanner)
set(_expected "${_plugins}/libgstapp.dll;${_plugins}/libgstplayback.dll")
if(NOT _actual STREQUAL _expected OR NOT _scanner STREQUAL _helper)
    message(FATAL_ERROR "Unexpected selected media payload: ${_actual}; scanner=${_scanner}")
endif()
