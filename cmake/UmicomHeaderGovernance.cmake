#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomHeaderGovernance.cmake
#
# PURPOSE:
#   Register source-tree audits that protect public header comments, include
#   guards and include-order independence without creating thousands of tests.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

function(umicom_register_header_governance_test test_name header_root owner)
    if(NOT BUILD_TESTING)
        return()
    endif()
    add_test(
        NAME "${test_name}"
        COMMAND "${CMAKE_COMMAND}"
            "-DUMICOM_HEADER_ROOT=${header_root}"
            "-DUMICOM_HEADER_OWNER=${owner}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomPublicHeaderAudit.cmake")
    set_tests_properties("${test_name}" PROPERTIES
        LABELS "headers;sdk;contracts;include-guards;documentation")
endfunction()

function(umicom_register_header_portfolio_governance_test test_name owner)
    if(NOT BUILD_TESTING)
        return()
    endif()
    set(UMICOM_HEADER_ROOTS ${ARGN})
    if(NOT UMICOM_HEADER_ROOTS)
        message(FATAL_ERROR
            "${test_name} requires at least one public include directory.")
    endif()
    string(JOIN "|" UMICOM_HEADER_ROOT_ARGUMENT ${UMICOM_HEADER_ROOTS})
    add_test(
        NAME "${test_name}"
        COMMAND "${CMAKE_COMMAND}"
            "-DUMICOM_HEADER_ROOTS=${UMICOM_HEADER_ROOT_ARGUMENT}"
            "-DUMICOM_HEADER_OWNER=${owner}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomPublicHeaderAudit.cmake")
    set_tests_properties("${test_name}" PROPERTIES
        LABELS "headers;sdk;contracts;include-guards;documentation;portfolio")
endfunction()
