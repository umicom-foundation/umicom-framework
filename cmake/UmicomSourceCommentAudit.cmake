#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSourceCommentAudit.cmake
#
# PURPOSE:
#   Audit C implementation and include-fragment files for the human-readable
#   purpose, author and licence information required by the coding convention.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED UMICOM_SOURCE_ROOTS OR UMICOM_SOURCE_ROOTS STREQUAL "")
    message(FATAL_ERROR "UMICOM_SOURCE_ROOTS must name at least one source directory.")
endif()
# Use the stable identifier comparison to choose the matching record or policy.
if(NOT DEFINED UMICOM_SOURCE_OWNER OR UMICOM_SOURCE_OWNER STREQUAL "")
    set(UMICOM_SOURCE_OWNER "Umicom source tree")
endif()

string(REPLACE "|" ";" UMICOM_AUDIT_ROOTS "${UMICOM_SOURCE_ROOTS}")
set(UMICOM_SOURCE_FILES)
# Visit each bounded item once so every record receives the same rule.
foreach(UMICOM_AUDIT_ROOT IN LISTS UMICOM_AUDIT_ROOTS)
    # Apply this branch only when its contract condition is satisfied.
    if(NOT IS_DIRECTORY "${UMICOM_AUDIT_ROOT}")
        continue()
    endif()
    file(GLOB_RECURSE UMICOM_ROOT_SOURCES LIST_DIRECTORIES FALSE
        "${UMICOM_AUDIT_ROOT}/*.c"
        "${UMICOM_AUDIT_ROOT}/*.inc")
    list(FILTER UMICOM_ROOT_SOURCES EXCLUDE REGEX
        "[/\\\\](build([-/\\\\]|$)|\\.git([/\\\\]|$))")
    list(APPEND UMICOM_SOURCE_FILES ${UMICOM_ROOT_SOURCES})
endforeach()
list(REMOVE_DUPLICATES UMICOM_SOURCE_FILES)
list(SORT UMICOM_SOURCE_FILES)

# Apply this branch only when its contract condition is satisfied.
if(NOT UMICOM_SOURCE_FILES)
    message(FATAL_ERROR "No C implementation files were found in the supplied roots.")
endif()

set(UMICOM_PROBLEMS)
# Visit each bounded item once so every record receives the same rule.
foreach(UMICOM_SOURCE_FILE IN LISTS UMICOM_SOURCE_FILES)
    file(READ "${UMICOM_SOURCE_FILE}" UMICOM_SOURCE_TEXT LIMIT 8192)
    string(REPLACE "\\" "/" UMICOM_SOURCE_LABEL "${UMICOM_SOURCE_FILE}")
    string(FIND "${UMICOM_SOURCE_TEXT}" "/*-----------------------------------------------------------------------------"
        UMICOM_BANNER_POSITION)
    # Use the stable identifier comparison to choose the matching record or policy.
    if(NOT UMICOM_BANNER_POSITION EQUAL 0)
        list(APPEND UMICOM_PROBLEMS
            "${UMICOM_SOURCE_LABEL}: the Umicom file comment is not at the top")
    endif()
    # Visit each bounded item once so every record receives the same rule.
    foreach(UMICOM_REQUIRED_TEXT
            "File:"
            "PURPOSE:"
            "AUTHOR AND ORGANISATION:"
            "Sammy Hegab"
            "Umicom Foundation"
            "LICENCE:"
            "MIT")
        string(FIND "${UMICOM_SOURCE_TEXT}" "${UMICOM_REQUIRED_TEXT}"
            UMICOM_REQUIRED_POSITION)
        # Use the stable identifier comparison to choose the matching record or policy.
        if(UMICOM_REQUIRED_POSITION EQUAL -1)
            list(APPEND UMICOM_PROBLEMS
                "${UMICOM_SOURCE_LABEL}: missing ${UMICOM_REQUIRED_TEXT}")
        endif()
    endforeach()
endforeach()

list(LENGTH UMICOM_PROBLEMS UMICOM_PROBLEM_COUNT)
list(LENGTH UMICOM_SOURCE_FILES UMICOM_SOURCE_COUNT)
# Apply this branch only when its contract condition is satisfied.
if(UMICOM_PROBLEM_COUNT GREATER 0)
    list(JOIN UMICOM_PROBLEMS "\n  - " UMICOM_PROBLEM_TEXT)
    message(FATAL_ERROR
        "${UMICOM_SOURCE_OWNER} source comment audit found "
        "${UMICOM_PROBLEM_COUNT} problem(s):\n  - ${UMICOM_PROBLEM_TEXT}")
endif()

message(STATUS
    "${UMICOM_SOURCE_OWNER}: ${UMICOM_SOURCE_COUNT} implementation files "
    "have complete human-readable file comments.")
