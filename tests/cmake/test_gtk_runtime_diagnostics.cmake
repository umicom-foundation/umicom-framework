#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/test_gtk_runtime_diagnostics.cmake
#
# PURPOSE:
#   Check toolkit advisory classification and one-time reporting without GTK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED FRAMEWORK_ROOT)
    message(FATAL_ERROR "FRAMEWORK_ROOT is required")
endif()
include("${FRAMEWORK_ROOT}/cmake/UmicomGtkRuntimeDiagnostics.cmake")
if(DEFINED EMIT_ADVISORY)
    umicom_report_gtk_runtime_advisories("4.24.0" "Windows")
    umicom_report_gtk_runtime_advisories("4.24.0" "Windows")
    return()
endif()

function(Check version system expected)
    umicom_gtk_runtime_advisories("${version}" "${system}" _actual)
    if(NOT _actual STREQUAL expected)
        message(FATAL_ERROR "Wrong advisory for ${system}/${version}: ${_actual}")
    endif()
endfunction()
Check("4.24.0" "Windows" "gtk.win32.ime-disposal")
Check("4.24.1" "Windows" "gtk.win32.ime-disposal")
Check("4.24.0-vendor.1" "Windows" "gtk.win32.ime-disposal")
Check("4.24.1.2" "Windows" "gtk.win32.ime-disposal")
Check("4.24.0" "Linux" "")
Check("4.24.1" "Darwin" "")
Check("4.22.0" "Windows" "")
Check("4.24.10" "Windows" "")
# No catalogue entry does not assert that an unreviewed version is safe.
Check("4.25.0" "Windows" "")
Check("" "Windows" "gtk.runtime.version-unavailable")
Check("unknown" "Windows" "gtk.runtime.version-unavailable")
Check("4.24" "Windows" "gtk.runtime.version-unavailable")

execute_process(COMMAND "${CMAKE_COMMAND}" "-DFRAMEWORK_ROOT=${FRAMEWORK_ROOT}"
    -DEMIT_ADVISORY=ON -P "${CMAKE_CURRENT_LIST_FILE}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Advisory emission changed configuration success: ${_error}")
endif()
string(REGEX REPLACE "[ \t\r\n]+" " " _normalized "${_error}")
string(REGEX MATCHALL "GTK 4[.]24[.]0 on Windows needs" _occurrences "${_normalized}")
list(LENGTH _occurrences _count)
if(NOT _count EQUAL 1 OR NOT _normalized MATCHES "Tests remain enabled" OR
   NOT _normalized MATCHES "30727f8a524dbaa69118a60d6bbef4b3447245b1")
    message(FATAL_ERROR "Expected one actionable advisory with upstream identity: ${_error}")
endif()
message(STATUS "GTK advisory catalogue checked; native runtime behavior requires separate qualification")
