#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationSurfaceAudit.cmake
#
# PURPOSE:
#   Check that each application folder has a CMake entry and a headless start
#   source, then write a small machine-readable report for release checks.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED UMICOM_APPLICATION_ROOT OR
   NOT IS_DIRECTORY "${UMICOM_APPLICATION_ROOT}")
    message(FATAL_ERROR
        "UMICOM_APPLICATION_ROOT must point to the applications directory")
endif()

if(NOT DEFINED UMICOM_SURFACE_AUDIT_OUTPUT OR
   "${UMICOM_SURFACE_AUDIT_OUTPUT}" STREQUAL "")
    set(UMICOM_SURFACE_AUDIT_OUTPUT
        "${CMAKE_BINARY_DIR}/umicom-application-surface-audit.json")
endif()

file(GLOB _application_children LIST_DIRECTORIES true
    "${UMICOM_APPLICATION_ROOT}/*")
list(SORT _application_children)
set(_application_count 0)
set(_ready_count 0)
set(_issues "")
set(_rows "")

foreach(_application_path IN LISTS _application_children)
    if(NOT IS_DIRECTORY "${_application_path}")
        continue()
    endif()
    get_filename_component(_application_name "${_application_path}" NAME)
    if(_application_name MATCHES "^\\.")
        continue()
    endif()
    math(EXPR _application_count "${_application_count}+1")
    set(_has_cmake FALSE)
    set(_has_console FALSE)
    set(_declares_console FALSE)
    if(EXISTS "${_application_path}/CMakeLists.txt")
        set(_has_cmake TRUE)
        file(READ "${_application_path}/CMakeLists.txt" _application_cmake)
        if(_application_cmake MATCHES "src/console/main\\.c")
            set(_declares_console TRUE)
        endif()
    endif()
    if(EXISTS "${_application_path}/src/console/main.c")
        set(_has_console TRUE)
    endif()
    if(_has_cmake AND _has_console AND _declares_console)
        math(EXPR _ready_count "${_ready_count}+1")
        set(_state "ready")
    else()
        set(_state "missing-start-surface")
        if(NOT _has_cmake)
            string(APPEND _issues "${_application_name}: missing CMakeLists.txt; ")
        endif()
        if(NOT _has_console)
            string(APPEND _issues "${_application_name}: missing src/console/main.c; ")
        endif()
        if(_has_console AND NOT _declares_console)
            string(APPEND _issues "${_application_name}: console source is not declared in CMakeLists.txt; ")
        endif()
    endif()
    string(APPEND _rows
        "    {\"application\":\"${_application_name}\",\"state\":\"${_state}\"},\n")
endforeach()

if(_application_count EQUAL 0)
    message(FATAL_ERROR "No application directories were found")
endif()

string(REGEX REPLACE ",\n$" "\n" _rows "${_rows}")
file(WRITE "${UMICOM_SURFACE_AUDIT_OUTPUT}"
    "{\n"
    "  \"application_count\": ${_application_count},\n"
    "  \"ready_count\": ${_ready_count},\n"
    "  \"applications\": [\n"
    "${_rows}"
    "  ]\n"
    "}\n")

if(_issues)
    message(FATAL_ERROR "Application surface audit failed: ${_issues}")
endif()
message(STATUS
    "Application surface audit passed: ${_ready_count}/${_application_count} start surfaces")
