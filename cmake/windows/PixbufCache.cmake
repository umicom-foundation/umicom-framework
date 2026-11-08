#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/PixbufCache.cmake
#
# PURPOSE:
#   Keep conventional Windows image-loader caches relative to their runtime.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# GdkPixbuf's relocatable Windows build resolves relative module records from
# its own DLL prefix. Native test hosts need this conventional cache even when
# they do not use the Umicom application constructor. Preserve format metadata;
# only module-path records are normalised. No module is loaded by this helper.
function(umicom_windows_relative_pixbuf_cache cache prefix version output)
    string(REPLACE "\\" "/" _prefix "${prefix}")
    string(REGEX REPLACE "/+$" "" _prefix "${_prefix}")
    string(TOLOWER "${_prefix}/" _prefix_lower)
    string(LENGTH "${_prefix}/" _prefix_length)
    if(NOT version MATCHES "^[A-Za-z0-9._-]+$" OR version STREQUAL "." OR version STREQUAL "..")
        message(FATAL_ERROR "Invalid image-loader ABI directory")
    endif()
    string(REPLACE "\r\n" "\n" _text "${cache}")
    string(REPLACE ";" "\\;" _lines "${_text}")
    string(REPLACE "\n" ";" _lines "${_lines}")
    set(_result "")
    set(_paths "")
    foreach(_line IN LISTS _lines)
        if(_line MATCHES "^\"([^\"]+[.][dD][lL][lL])\"$")
            set(_module "${CMAKE_MATCH_1}")
            # query-loaders escapes Windows backslashes in its quoted strings.
            string(REPLACE "\\\\" "/" _module "${_module}")
            string(TOLOWER "${_module}" _module_lower)
            string(FIND "${_module_lower}" "${_prefix_lower}" _prefix_at)
            if(_prefix_at EQUAL 0)
                string(SUBSTRING "${_module}" ${_prefix_length} -1 _module)
            endif()
            if(NOT _module MATCHES "^lib/gdk-pixbuf-2[.]0/([^/]+)/loaders/[^/]+[.][dD][lL][lL]$")
                message(FATAL_ERROR "Image-loader cache names a foreign or malformed module: ${_module}")
            endif()
            set(_module_version "${CMAKE_MATCH_1}")
            if(NOT _module_version STREQUAL version OR _module MATCHES "[;\r\n\t\\\\]"
                    OR _module MATCHES "(^|/)[.][.]?(/|$)")
                message(FATAL_ERROR "Image-loader cache names a foreign or malformed module: ${_module}")
            endif()
            string(TOLOWER "${_module}" _identity)
            if(_identity IN_LIST _paths)
                message(FATAL_ERROR "Image-loader cache repeats a module: ${_module}")
            endif()
            list(APPEND _paths "${_identity}")
            string(APPEND _result "\"${_module}\"\n")
        else()
            string(APPEND _result "${_line}\n")
        endif()
    endforeach()
    if(NOT _paths)
        message(FATAL_ERROR "Image-loader cache contains no module records")
    endif()
    set(${output} "${_result}" PARENT_SCOPE)
endfunction()
