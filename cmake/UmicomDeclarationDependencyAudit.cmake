#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeclarationDependencyAudit.cmake
#
# PURPOSE:
#   Catch catalogue API calls whose public declaration is not visible through
#   a direct include or an approved aggregate include before compilation.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED UMICOM_DECLARATION_ROOTS OR UMICOM_DECLARATION_ROOTS STREQUAL "")
    message(FATAL_ERROR "UMICOM_DECLARATION_ROOTS must name source directories.")
endif()
string(REPLACE "|" ";" UMICOM_AUDIT_ROOTS "${UMICOM_DECLARATION_ROOTS}")
set(UMICOM_SOURCE_FILES)
# Visit each bounded item once so every record receives the same rule.
foreach(UMICOM_AUDIT_ROOT IN LISTS UMICOM_AUDIT_ROOTS)
    # Apply this branch only when its contract condition is satisfied.
    if(IS_DIRECTORY "${UMICOM_AUDIT_ROOT}")
        file(GLOB_RECURSE UMICOM_ROOT_SOURCES LIST_DIRECTORIES FALSE
            "${UMICOM_AUDIT_ROOT}/*.c")
        list(FILTER UMICOM_ROOT_SOURCES EXCLUDE REGEX
            "[/\\\\](build([-/\\\\]|$)|\\.git([/\\\\]|$))")
        list(APPEND UMICOM_SOURCE_FILES ${UMICOM_ROOT_SOURCES})
    endif()
endforeach()
list(REMOVE_DUPLICATES UMICOM_SOURCE_FILES)

set(UMICOM_PROBLEMS)
# Define the check declaration dependency build helper so parent and application projects
# apply one consistent rule.
function(umicom_check_declaration_dependency prefix contract_name)
    set(UMICOM_ACCEPTED_HEADERS ${ARGN})
    # Visit each bounded item once so every record receives the same rule.
    foreach(UMICOM_SOURCE_FILE IN LISTS UMICOM_SOURCE_FILES)
        file(READ "${UMICOM_SOURCE_FILE}" UMICOM_SOURCE_TEXT)
        string(FIND "${UMICOM_SOURCE_TEXT}" "${prefix}" UMICOM_CALL_POSITION)
        # Use the stable identifier comparison to choose the matching record or policy.
        if(UMICOM_CALL_POSITION EQUAL -1)
            continue()
        endif()
        set(UMICOM_DECLARATION_VISIBLE FALSE)
        # Visit each bounded item once so every record receives the same rule.
        foreach(UMICOM_ACCEPTED_HEADER IN LISTS UMICOM_ACCEPTED_HEADERS)
            string(FIND "${UMICOM_SOURCE_TEXT}"
                "\"${UMICOM_ACCEPTED_HEADER}\"" UMICOM_INCLUDE_POSITION)
            # Use the stable identifier comparison to choose the matching record or policy.
            if(NOT UMICOM_INCLUDE_POSITION EQUAL -1)
                set(UMICOM_DECLARATION_VISIBLE TRUE)
                break()
            endif()
        endforeach()
        # Apply this operation only while the related capability or state is available.
        if(NOT UMICOM_DECLARATION_VISIBLE)
            string(REPLACE "\\" "/" UMICOM_SOURCE_LABEL
                "${UMICOM_SOURCE_FILE}")
            list(APPEND UMICOM_PROBLEMS
                "${UMICOM_SOURCE_LABEL}: ${contract_name} declaration is not included")
        endif()
    endforeach()
    set(UMICOM_PROBLEMS "${UMICOM_PROBLEMS}" PARENT_SCOPE)
endfunction()

umicom_check_declaration_dependency(
    "umi_application_component_recipe_catalogue_"
    "application component recipe catalogue"
    "umicom/application/component/recipe_catalogue.h"
    "umicom/application/component/component.h"
    "umicom/application/application.h"
    "umicom/umicom.h")
umicom_check_declaration_dependency(
    "umi_application_experience_profile_catalogue_"
    "application experience profile catalogue"
    "umicom/application/experience/catalogue.h"
    "umicom/application/experience/experience.h"
    "umicom/application/experience_portfolio.h"
    "umicom/application/application.h"
    "umicom/umicom.h")
umicom_check_declaration_dependency(
    "umi_application_experience_catalogue_"
    "application experience definition catalogue"
    "umicom/application/experience_catalogue.h"
    "umicom/application/experience_portfolio.h"
    "umicom/application/application.h"
    "umicom/umicom.h")

list(LENGTH UMICOM_PROBLEMS UMICOM_PROBLEM_COUNT)
# Apply this branch only when its contract condition is satisfied.
if(UMICOM_PROBLEM_COUNT GREATER 0)
    list(JOIN UMICOM_PROBLEMS "\n  - " UMICOM_PROBLEM_TEXT)
    message(FATAL_ERROR
        "Catalogue declaration dependency audit found ${UMICOM_PROBLEM_COUNT} "
        "problem(s):\n  - ${UMICOM_PROBLEM_TEXT}")
endif()
list(LENGTH UMICOM_SOURCE_FILES UMICOM_SOURCE_COUNT)
message(STATUS
    "Catalogue declaration dependencies are visible across "
    "${UMICOM_SOURCE_COUNT} C implementation files.")
