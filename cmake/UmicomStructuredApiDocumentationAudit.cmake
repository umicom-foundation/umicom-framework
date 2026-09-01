#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomStructuredApiDocumentationAudit.cmake
#
# PURPOSE:
#   Check that every public function in selected headers has its own nearby
#   structured comment so generated HTML never relies on unrelated text.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

# The caller chooses a focused public-header tree so documentation can improve
# one complete subsystem at a time without hiding older coverage gaps.
if(NOT DEFINED UMICOM_DOCUMENTATION_HEADER_ROOT OR
   UMICOM_DOCUMENTATION_HEADER_ROOT STREQUAL "")
    message(FATAL_ERROR
        "UMICOM_DOCUMENTATION_HEADER_ROOT must name a public header folder.")
endif()

# A configurable prefix lets the same audit support other Framework modules.
if(NOT DEFINED UMICOM_DOCUMENTATION_API_PREFIX OR
   UMICOM_DOCUMENTATION_API_PREFIX STREQUAL "")
    set(UMICOM_DOCUMENTATION_API_PREFIX "umi_")
endif()

# A missing folder usually means a stale build rule and must not pass silently.
if(NOT IS_DIRECTORY "${UMICOM_DOCUMENTATION_HEADER_ROOT}")
    message(FATAL_ERROR
        "Documentation header folder was not found: "
        "${UMICOM_DOCUMENTATION_HEADER_ROOT}")
endif()

file(GLOB_RECURSE UMICOM_DOCUMENTATION_HEADERS
     LIST_DIRECTORIES FALSE
     "${UMICOM_DOCUMENTATION_HEADER_ROOT}/*.h")
list(SORT UMICOM_DOCUMENTATION_HEADERS)

# An empty audit would provide false confidence, so require at least one header.
if(NOT UMICOM_DOCUMENTATION_HEADERS)
    message(FATAL_ERROR
        "No public headers were found below "
        "${UMICOM_DOCUMENTATION_HEADER_ROOT}.")
endif()

set(UMICOM_DOCUMENTATION_PROBLEMS)
set(UMICOM_DOCUMENTATION_FUNCTION_COUNT 0)

# Read headers as lines so comments can be matched to the declaration directly
# below them without attempting to become a full C parser.
foreach(UMICOM_DOCUMENTATION_HEADER IN LISTS UMICOM_DOCUMENTATION_HEADERS)
    file(STRINGS "${UMICOM_DOCUMENTATION_HEADER}"
         UMICOM_DOCUMENTATION_LINES)
    set(UMICOM_DOCUMENTATION_IN_COMMENT FALSE)
    set(UMICOM_DOCUMENTATION_COMMENT_DISTANCE 1000)
    set(UMICOM_DOCUMENTATION_LINE_NUMBER 0)

    foreach(UMICOM_DOCUMENTATION_LINE IN LISTS UMICOM_DOCUMENTATION_LINES)
        math(EXPR UMICOM_DOCUMENTATION_LINE_NUMBER
             "${UMICOM_DOCUMENTATION_LINE_NUMBER} + 1")
        string(STRIP "${UMICOM_DOCUMENTATION_LINE}"
               UMICOM_DOCUMENTATION_TRIMMED)

        # A double-star comment is the source used by generated API pages.
        if(UMICOM_DOCUMENTATION_TRIMMED MATCHES "^/\\*\\*")
            set(UMICOM_DOCUMENTATION_IN_COMMENT TRUE)
            set(UMICOM_DOCUMENTATION_COMMENT_DISTANCE 0)
        endif()

        # Lines inside the structured block may mention function names as
        # relationships, but those references are not declarations to audit.
        if(UMICOM_DOCUMENTATION_IN_COMMENT)
            if(UMICOM_DOCUMENTATION_TRIMMED MATCHES "\\*/")
                set(UMICOM_DOCUMENTATION_IN_COMMENT FALSE)
                set(UMICOM_DOCUMENTATION_COMMENT_DISTANCE 0)
            endif()
            continue()
        endif()

        # Empty lines keep a completed documentation block attached to the
        # declaration while allowing readable spacing in public headers.
        if(UMICOM_DOCUMENTATION_TRIMMED STREQUAL "")
            continue()
        endif()

        # Public Umicom function names are stable and make a focused audit
        # reliable without interpreting structures, macros or callback types.
        if(UMICOM_DOCUMENTATION_TRIMMED MATCHES
           "${UMICOM_DOCUMENTATION_API_PREFIX}[A-Za-z0-9_]*[ \\t]*\\(")
            math(EXPR UMICOM_DOCUMENTATION_FUNCTION_COUNT
                 "${UMICOM_DOCUMENTATION_FUNCTION_COUNT} + 1")
            # A return type may occupy the line between a comment and function
            # name. A larger gap means the comment belongs to another item.
            if(UMICOM_DOCUMENTATION_COMMENT_DISTANCE GREATER 1)
                string(CONCAT UMICOM_DOCUMENTATION_PROBLEM
                    "${UMICOM_DOCUMENTATION_HEADER}:"
                    "${UMICOM_DOCUMENTATION_LINE_NUMBER}: public function "
                    "lacks its own structured comment")
                list(APPEND UMICOM_DOCUMENTATION_PROBLEMS
                    "${UMICOM_DOCUMENTATION_PROBLEM}")
            endif()
        endif()

        # The semicolon closes a declaration and prevents its comment from
        # being reused accidentally by the next public function.
        if(UMICOM_DOCUMENTATION_TRIMMED MATCHES ";[ \\t]*$")
            set(UMICOM_DOCUMENTATION_COMMENT_DISTANCE 1000)
        elseif(UMICOM_DOCUMENTATION_COMMENT_DISTANCE LESS 1000)
            math(EXPR UMICOM_DOCUMENTATION_COMMENT_DISTANCE
                 "${UMICOM_DOCUMENTATION_COMMENT_DISTANCE} + 1")
        endif()
    endforeach()
endforeach()

list(LENGTH UMICOM_DOCUMENTATION_PROBLEMS
     UMICOM_DOCUMENTATION_PROBLEM_COUNT)

# A prefix that matches nothing is probably misspelled and should fail clearly.
if(UMICOM_DOCUMENTATION_FUNCTION_COUNT EQUAL 0)
    message(FATAL_ERROR
        "No public functions matched prefix "
        "${UMICOM_DOCUMENTATION_API_PREFIX}.")
endif()

# Report every missing contract together so a contributor can repair one
# coherent set of headers instead of repeating configure cycles.
if(UMICOM_DOCUMENTATION_PROBLEM_COUNT GREATER 0)
    list(JOIN UMICOM_DOCUMENTATION_PROBLEMS "\n  - "
         UMICOM_DOCUMENTATION_PROBLEM_TEXT)
    message(FATAL_ERROR
        "Structured API documentation audit found "
        "${UMICOM_DOCUMENTATION_PROBLEM_COUNT} problem(s):\n  - "
        "${UMICOM_DOCUMENTATION_PROBLEM_TEXT}")
endif()

list(LENGTH UMICOM_DOCUMENTATION_HEADERS UMICOM_DOCUMENTATION_HEADER_COUNT)
message(STATUS
    "Structured API documentation: ${UMICOM_DOCUMENTATION_FUNCTION_COUNT} "
    "public functions in ${UMICOM_DOCUMENTATION_HEADER_COUNT} headers are "
    "documented.")
