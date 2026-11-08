#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/test_pixbuf_cache.cmake
#
# PURPOSE:
#   Verify relative image-loader records preserve metadata and reject foreign paths.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT FRAMEWORK OR NOT CASE)
    message(FATAL_ERROR "FRAMEWORK and CASE are required")
endif()
include("${FRAMEWORK}/cmake/windows/PixbufCache.cmake")
set(_prefix "C:/SDK With Spaces/ucrt64")
set(_version "2.10.0")
set(_relative "lib/gdk-pixbuf-2.0/2.10.0/loaders/pixbufloader_svg.dll")
set(_module "${_relative}")
set(_negative FALSE)
if(CASE STREQUAL "forward")
    set(_module "${_prefix}/${_relative}")
elseif(CASE STREQUAL "escaped")
    string(REPLACE "/" "\\\\" _module "${_prefix}/${_relative}")
elseif(CASE STREQUAL "prefix-case")
    set(_module "c:/sdk with spaces/UCRT64/${_relative}")
elseif(CASE STREQUAL "relative" OR CASE STREQUAL "crlf" OR CASE STREQUAL "metadata")
elseif(CASE STREQUAL "foreign")
    set(_module "D:/other/${_relative}")
    set(_negative TRUE)
elseif(CASE STREQUAL "prefix-sibling")
    set(_module "${_prefix}-other/${_relative}")
    set(_negative TRUE)
elseif(CASE STREQUAL "traversal")
    set(_module "lib/gdk-pixbuf-2.0/../loaders/pixbufloader_svg.dll")
    set(_negative TRUE)
elseif(CASE STREQUAL "abi")
    set(_version "9.0.0")
    set(_negative TRUE)
elseif(CASE STREQUAL "duplicate" OR CASE STREQUAL "empty")
    set(_negative TRUE)
else()
    message(FATAL_ERROR "Unknown cache case")
endif()
set(_metadata "\"svg\" 6 \"gdk-pixbuf\" \"SVG; vector artwork\" \"LGPL\"\n\"image/svg+xml\" \"\"\n\"svg\" \"\"\n\" <svg\" \"*    \" 100\n")
set(_cache "# Fixture, not an executable decoder\n\"${_module}\"\n${_metadata}\n")
if(CASE STREQUAL "crlf")
    string(REPLACE "\n" "\r\n" _cache "${_cache}")
elseif(CASE STREQUAL "metadata")
    # A second format proves that records remain distinct while metadata with
    # a semicolon and an empty terminator survives CMake's list representation.
    string(APPEND _cache "\"lib/gdk-pixbuf-2.0/2.10.0/loaders/bmp.dll\"\n\"bmp\" 5 \"gdk-pixbuf\" \"Bitmap\" \"LGPL\"\n\"image/bmp\" \"\"\n\"bmp\" \"\"\n\"BM\" \"\" 100\n\n")
elseif(CASE STREQUAL "duplicate")
    string(APPEND _cache "\"${_module}\"\n${_metadata}\n")
elseif(CASE STREQUAL "empty")
    set(_cache "# No decoder records\n")
endif()
# Inspect the error in a child so an unrelated crash or syntax failure cannot
# satisfy a test whose expected result is a rejected cache record.
if(_negative AND NOT CHILD)
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DFRAMEWORK=${FRAMEWORK}" "-DCASE=${CASE}"
        -DCHILD=ON -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE _status OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr)
    if(_status STREQUAL "0" OR NOT _stderr MATCHES "Image-loader cache (names|repeats|contains)")
        message(FATAL_ERROR "Expected bounded cache rejection: ${_status}\n${_stdout}\n${_stderr}")
    endif()
    return()
endif()
umicom_windows_relative_pixbuf_cache("${_cache}" "${_prefix}" "${_version}" _result)
string(FIND "${_result}" "\"${_relative}\"\n" _path_at)
string(FIND "${_result}" "${_metadata}" _metadata_at)
if(_path_at LESS 0 OR _metadata_at LESS 0 OR _result MATCHES "[Cc]:|[Dd]:|\r")
    message(FATAL_ERROR "Cache path or format metadata changed unexpectedly: ${_result}")
endif()

if(CASE STREQUAL "metadata")
    string(FIND "${_result}" "\"lib/gdk-pixbuf-2.0/2.10.0/loaders/bmp.dll\"\n\"bmp\" 5" _second_at)
    if(_second_at LESS 0)
        message(FATAL_ERROR "The second decoder record was not preserved")
    endif()
endif()
