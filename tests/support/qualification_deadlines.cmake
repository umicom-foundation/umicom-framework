#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/support/qualification_deadlines.cmake
#
# PURPOSE:
#   Keep nested qualification steps bounded without truncating a healthy workflow.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
# Share finite stage limits between registration and execution. A complete
# nested build or suite needs a different budget from a single unit test.
function(umicom_qualification_stage_timeout output stage)
    if(stage MATCHES "^(configure|client-configure|reconfigure-[1-4]-(immediate|deferred))$")
        set(_seconds 180)
    elseif(stage MATCHES "^(build|client-build|regenerate-build|rebuild-[1-4]-(immediate|deferred))$")
        set(_seconds 600)
    elseif(stage STREQUAL "tests")
        set(_seconds 600)
    elseif(stage MATCHES "^(inventory|regenerated-inventory|reinventory-[1-4]-(immediate|deferred))$")
        set(_seconds 60)
    elseif(stage STREQUAL "install" OR stage STREQUAL "client-test")
        set(_seconds 180)
    elseif(stage STREQUAL "wait-for-stamp")
        set(_seconds 10)
    else()
        message(FATAL_ERROR "Unknown qualification stage: ${stage}")
    endif()
    set(${output} "${_seconds}" PARENT_SCOPE)
endfunction()

function(umicom_qualification_workflow_timeout output)
    if(NOT ARGN)
        message(FATAL_ERROR "A qualification workflow must contain a stage")
    endif()
    set(_total 30)
    foreach(_stage IN LISTS ARGN)
        umicom_qualification_stage_timeout(_seconds "${_stage}")
        math(EXPR _total "${_total} + ${_seconds}")
    endforeach()
    set(${output} "${_total}" PARENT_SCOPE)
endfunction()

function(umicom_component_integration_timeout output scenario)
    set(_known deferred immediate mixed repeated missing partial tests_off reconfigure regenerate installed)
    if(NOT scenario IN_LIST _known)
        message(FATAL_ERROR "Unknown integration scenario: ${scenario}")
    endif()
    set(_stages configure build inventory)
    if(scenario STREQUAL "reconfigure")
        foreach(_index RANGE 1 4)
            list(APPEND _stages reconfigure-${_index}-immediate
                rebuild-${_index}-immediate reinventory-${_index}-immediate)
        endforeach()
    elseif(scenario STREQUAL "regenerate")
        list(APPEND _stages wait-for-stamp regenerate-build regenerated-inventory)
    endif()
    if(NOT scenario STREQUAL "missing" AND NOT scenario STREQUAL "tests_off")
        list(APPEND _stages tests)
    endif()
    if(scenario STREQUAL "installed")
        list(APPEND _stages install client-configure client-build client-test)
    endif()
    umicom_qualification_workflow_timeout(_total ${_stages})
    set(${output} "${_total}" PARENT_SCOPE)
endfunction()
