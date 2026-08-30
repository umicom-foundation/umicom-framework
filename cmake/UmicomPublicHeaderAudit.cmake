#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomPublicHeaderAudit.cmake
#
# PURPOSE:
#   Audit every public header for a unique matching include guard and the
#   human-readable Umicom file comment required by the coding convention.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

if(DEFINED UMICOM_HEADER_ROOTS AND NOT UMICOM_HEADER_ROOTS STREQUAL "")
    string(REPLACE "|" ";" UMICOM_AUDIT_ROOTS "${UMICOM_HEADER_ROOTS}")
elseif(DEFINED UMICOM_HEADER_ROOT AND NOT UMICOM_HEADER_ROOT STREQUAL "")
    set(UMICOM_AUDIT_ROOTS "${UMICOM_HEADER_ROOT}")
else()
    message(FATAL_ERROR
        "UMICOM_HEADER_ROOT or UMICOM_HEADER_ROOTS must name a public include directory.")
endif()
if(NOT DEFINED UMICOM_HEADER_OWNER OR UMICOM_HEADER_OWNER STREQUAL "")
    set(UMICOM_HEADER_OWNER "Umicom SDK")
endif()

set(UMICOM_PUBLIC_HEADERS)
foreach(UMICOM_AUDIT_ROOT IN LISTS UMICOM_AUDIT_ROOTS)
    if(NOT IS_DIRECTORY "${UMICOM_AUDIT_ROOT}")
        message(FATAL_ERROR
            "Public header directory does not exist: ${UMICOM_AUDIT_ROOT}")
    endif()
    file(GLOB_RECURSE UMICOM_ROOT_HEADERS
        LIST_DIRECTORIES FALSE
        "${UMICOM_AUDIT_ROOT}/*.h")
    list(APPEND UMICOM_PUBLIC_HEADERS ${UMICOM_ROOT_HEADERS})
endforeach()
list(REMOVE_DUPLICATES UMICOM_PUBLIC_HEADERS)
list(SORT UMICOM_PUBLIC_HEADERS)

if(NOT UMICOM_PUBLIC_HEADERS)
    message(FATAL_ERROR "No public headers were found under ${UMICOM_HEADER_ROOT}.")
endif()

set(UMICOM_PROBLEMS)

foreach(UMICOM_HEADER IN LISTS UMICOM_PUBLIC_HEADERS)
    file(READ "${UMICOM_HEADER}" UMICOM_HEADER_TEXT LIMIT 131072)
    if(DEFINED UMICOM_HEADER_ROOT AND NOT UMICOM_HEADER_ROOT STREQUAL "")
        file(RELATIVE_PATH UMICOM_HEADER_RELATIVE
            "${UMICOM_HEADER_ROOT}" "${UMICOM_HEADER}")
    else()
        set(UMICOM_HEADER_RELATIVE "${UMICOM_HEADER}")
    endif()
    string(REPLACE "\\" "/" UMICOM_HEADER_RELATIVE
        "${UMICOM_HEADER_RELATIVE}")

    foreach(UMICOM_REQUIRED_TEXT
            "File:"
            "PURPOSE:"
            "AUTHOR AND ORGANISATION:"
            "LICENCE:")
        string(FIND "${UMICOM_HEADER_TEXT}" "${UMICOM_REQUIRED_TEXT}"
            UMICOM_REQUIRED_POSITION)
        if(UMICOM_REQUIRED_POSITION EQUAL -1)
            list(APPEND UMICOM_PROBLEMS
                "${UMICOM_HEADER_RELATIVE}: missing ${UMICOM_REQUIRED_TEXT}")
        endif()
    endforeach()

    set(CMAKE_MATCH_1 "")
    string(REGEX MATCH
        "#[ \t]*ifndef[ \t]+([A-Za-z_][A-Za-z0-9_]*)"
        UMICOM_IFNDEF_MATCH "${UMICOM_HEADER_TEXT}")
    set(UMICOM_GUARD "${CMAKE_MATCH_1}")
    if(UMICOM_GUARD STREQUAL "")
        list(APPEND UMICOM_PROBLEMS
            "${UMICOM_HEADER_RELATIVE}: missing include guard")
        continue()
    endif()

    string(REGEX MATCH
        "#[ \t]*define[ \t]+${UMICOM_GUARD}([ \t\r\n]|$)"
        UMICOM_DEFINE_MATCH "${UMICOM_HEADER_TEXT}")
    if(UMICOM_DEFINE_MATCH STREQUAL "")
        list(APPEND UMICOM_PROBLEMS
            "${UMICOM_HEADER_RELATIVE}: #ifndef and #define do not match")
    endif()

    set(UMICOM_GUARD_VARIABLE "UMICOM_GUARD_FILE_${UMICOM_GUARD}")
    if(DEFINED ${UMICOM_GUARD_VARIABLE})
        list(APPEND UMICOM_PROBLEMS
            "${UMICOM_HEADER_RELATIVE}: guard ${UMICOM_GUARD} already belongs to ${${UMICOM_GUARD_VARIABLE}}")
    else()
        set(${UMICOM_GUARD_VARIABLE} "${UMICOM_HEADER_RELATIVE}")
    endif()
endforeach()

list(LENGTH UMICOM_PROBLEMS UMICOM_PROBLEM_COUNT)
list(LENGTH UMICOM_PUBLIC_HEADERS UMICOM_HEADER_COUNT)
if(UMICOM_PROBLEM_COUNT GREATER 0)
    list(JOIN UMICOM_PROBLEMS "\n  - " UMICOM_PROBLEM_TEXT)
    message(FATAL_ERROR
        "${UMICOM_HEADER_OWNER} public header audit found "
        "${UMICOM_PROBLEM_COUNT} problem(s):\n  - ${UMICOM_PROBLEM_TEXT}")
endif()

message(STATUS
    "${UMICOM_HEADER_OWNER}: ${UMICOM_HEADER_COUNT} public headers have "
    "unique guards and complete human-readable file comments.")
