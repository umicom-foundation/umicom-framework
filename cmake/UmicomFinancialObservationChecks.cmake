#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomFinancialObservationChecks.cmake
#
# PURPOSE:
#   Qualify finite currency previews, bounded audit evidence and complete coverage snapshots.
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
function(umicom_add_financial_observation_checks domain)
    set(target umi-financial-${domain}-admission-test)
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/financial_platform/test_${domain}_admission.c")
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
    # Admission helpers affect every control-book consumer, including coverage.
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/financial_platform/control_query.h")
    endif()
    foreach(scenario IN LISTS ARGN)
        set(test framework.financial_platform.observation.${domain}.${scenario})
        add_test(NAME ${test} COMMAND ${target} ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;admission;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_add_financial_observation_checks(rate
    valid identity text rate count duplicate stored-text revision stale capacity query missing amount overflow underflow null)
umicom_add_financial_observation_checks(audit
    valid empty text flags count revision sequence-zero duplicate backward exhausted near-limit gaps stored-text capacity repeat-mark missing null)
umicom_add_financial_observation_checks(coverage
    valid rate-count relationship-count instrument-count trade-count position-count payment-count settlement-count audit-count revision rate-invalid audit-invalid null
    relationship_store-invalid instrument_link_store-invalid trade_control_book-invalid position_control_book-invalid payment_control_book-invalid settlement_book-invalid)

# Exercise the six owners used by Bank, Trader and TMS coverage observations.
umicom_add_financial_observation_checks(relationship_store
    valid fields count duplicate stored-field revision capacity query flags checked null)
umicom_add_financial_observation_checks(instrument_link_store
    valid fields count duplicate stored-field revision capacity query flags versions null)
umicom_add_financial_observation_checks(trade_control_book
    valid fields count duplicate stored-field revision capacity query numbers states transitions null)
umicom_add_financial_observation_checks(position_control_book
    valid fields count duplicate stored-field revision capacity query numbers versions checked null)
umicom_add_financial_observation_checks(payment_control_book
    valid fields count duplicate stored-field revision capacity query numbers flags states transitions null)
umicom_add_financial_observation_checks(settlement_book
    valid fields count duplicate stored-field revision capacity query numbers states transitions checked null)
