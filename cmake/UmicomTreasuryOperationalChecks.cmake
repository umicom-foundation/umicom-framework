#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTreasuryOperationalChecks.cmake
#
# PURPOSE:
#   Qualify bounded Treasury books and complete operational observations.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(NOT BUILD_TESTING OR NOT TARGET umicom_finance)
    return()
endif()
function(umicom_add_treasury_admission_checks domain)
    set(target umi-treasury-${domain}-admission-test)
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_${domain}_admission.c")
    target_link_libraries(${target} PRIVATE Umicom::finance)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/operational_fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/open_tms/operational_internal.h")
    endif()
    foreach(scenario IN LISTS ARGN)
        set(test framework.open_tms.operational.${domain}.${scenario})
        add_test(NAME ${test} COMMAND ${target} ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;treasury;admission;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_add_treasury_admission_checks(cash
    valid empty row-text row-numbers count stored-row duplicate revision stale capacity query currencies overflow null)
umicom_add_treasury_admission_checks(liquidity
    valid empty text numbers count order revision capacity overflow null)
umicom_add_treasury_admission_checks(position
    valid text numbers count duplicate revision stale capacity query currencies product-overflow sum-overflow spread-overflow null)
umicom_add_treasury_admission_checks(risk
    valid text numbers count duplicate revision stale capacity query overflow null)
umicom_add_treasury_admission_checks(settlement
    valid text numbers state count duplicate revision stale terminal capacity query null)
umicom_add_treasury_admission_checks(reconciliation
    valid text numbers count duplicate counters flags resolved-import revision capacity query overflow null)
umicom_add_treasury_admission_checks(dashboard
    empty valid cash-count liquidity-count position-count risk-count settlement-count accounting-count reconciliation-count cash-overflow position-overflow risk-nan accounting-balance reconciliation-counters forward-count forward-revision service-revision null)

# Qualify the same public consumer source used by the installed SDK lesson.
add_executable(umi-treasury-observations "${CMAKE_CURRENT_LIST_DIR}/../examples/treasury_observations/main.c")
target_link_libraries(umi-treasury-observations PRIVATE Umicom::finance)
set_target_properties(umi-treasury-observations PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-treasury-observations)
umicom_apply_sanitizers(umi-treasury-observations)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-treasury-observations)
endif()
foreach(scenario preview recovery reconciliation controls lifecycle)
    set(test framework.open_tms.operational.consumer.${scenario})
    add_test(NAME ${test} COMMAND umi-treasury-observations ${scenario})
    set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;treasury;consumer;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umi-treasury-observations)
    endif()
endforeach()
