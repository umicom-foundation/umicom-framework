#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/component_integration/check_deadlines.cmake
#
# PURPOSE:
#   Verify workflow sums, stage bounds and rejection of unknown operations.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
include("${CMAKE_CURRENT_LIST_DIR}/../support/qualification_deadlines.cmake")
if(MODE STREQUAL "unknown-stage")
    umicom_qualification_stage_timeout(_value unexpected)
elseif(MODE STREQUAL "unknown-case")
    umicom_component_integration_timeout(_value unexpected)
elseif(MODE STREQUAL "empty-workflow")
    umicom_qualification_workflow_timeout(_value)
elseif(MODE STREQUAL "valid")
    foreach(_pair IN ITEMS "configure:180" "build:600" "tests:600" "inventory:60"
            "install:180" "client-configure:180" "client-build:600" "client-test:180"
            "wait-for-stamp:10" "regenerate-build:600" "regenerated-inventory:60"
            "reconfigure-4-deferred:180" "rebuild-4-deferred:600" "reinventory-4-deferred:60")
        string(REPLACE ":" ";" _fields "${_pair}")
        list(GET _fields 0 _stage)
        list(GET _fields 1 _expected)
        umicom_qualification_stage_timeout(_actual "${_stage}")
        if(NOT _actual EQUAL _expected)
            message(FATAL_ERROR "${_stage}: expected ${_expected}, got ${_actual}")
        endif()
    endforeach()
    foreach(_pair IN ITEMS "deferred:1470" "immediate:1470" "mixed:1470"
            "repeated:1470" "partial:1470" "missing:870" "tests_off:870"
            "reconfigure:4830" "regenerate:2140" "installed:2610")
        string(REPLACE ":" ";" _fields "${_pair}")
        list(GET _fields 0 _case)
        list(GET _fields 1 _expected)
        umicom_component_integration_timeout(_actual "${_case}")
        if(NOT _actual EQUAL _expected)
            message(FATAL_ERROR "${_case}: expected ${_expected}, got ${_actual}")
        endif()
    endforeach()
    umicom_qualification_workflow_timeout(_actual configure build tests)
    if(NOT _actual EQUAL 1410)
        message(FATAL_ERROR "Release composition must include all three stages and cleanup slack")
    endif()
else()
    message(FATAL_ERROR "Unknown deadline test mode: ${MODE}")
endif()
