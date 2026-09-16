#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomOperationalStateQualification.cmake
# PURPOSE: Exercise existing project, process, banking and trading state owners.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()

# Every case links production libraries. No stub service or alternative runtime
# is created by this verification helper; Release checks remain active in C.
function(umicom_add_operational_state_check suffix library source)
    set(test_target "umicom-operational-state-${suffix}-test")
    add_executable("${test_target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/operational_state/${source}")
    target_link_libraries("${test_target}" PRIVATE "${library}")
    umicom_apply_warnings("${test_target}")
    umicom_apply_sanitizers("${test_target}")
    if(ARGN)
        foreach(test_case IN LISTS ARGN)
            set(test_name "framework.operational_state.${suffix}.${test_case}")
            add_test(NAME "${test_name}" COMMAND "${test_target}" "${test_case}")
            set_tests_properties("${test_name}" PROPERTIES
                TIMEOUT 60 SKIP_RETURN_CODE 77 LABELS "framework;operational-state;regression")
        endforeach()
    else()
        add_test(NAME "framework.operational_state.${suffix}" COMMAND "${test_target}")
        set_tests_properties("framework.operational_state.${suffix}" PROPERTIES
            TIMEOUT 60 LABELS "framework;operational-state;regression")
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${test_target}")
    endif()
endfunction()

umicom_add_operational_state_check(profile-store Umicom::build test_profile_store.c
    roundtrip corrupt boundaries atomicity durable)
umicom_add_operational_state_check(process-retention Umicom::platform test_process_retention.c
    reuse concurrency shutdown)
umicom_add_operational_state_check(desk-exit Umicom::desktop test_desk_exit.c)
umicom_add_operational_state_check(financial-records Umicom::finance test_financial_records.c
    identifiers accounts book movements)
umicom_add_operational_state_check(trading-drafts Umicom::trading test_trading_drafts.c)
umicom_add_operational_state_check(directory-empty Umicom::platform test_directory_empty.c)
