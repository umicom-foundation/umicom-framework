#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTradingExecutionQualification.cmake
# PURPOSE: Link execution regressions to the existing trading libraries.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_add_trading_execution_checks group source)
    set(target "umicom-trading-execution-${group}-test")
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/trading_execution/${source}")
    target_link_libraries("${target}" PRIVATE Umicom::trading_ui)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    foreach(test_case IN LISTS ARGN)
        set(name "framework.trading_execution.${group}.${test_case}")
        add_test(NAME "${name}" COMMAND "${target}" "${test_case}")
        set_tests_properties("${name}" PROPERTIES TIMEOUT 30 LABELS "framework;trading;execution;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
umicom_add_trading_execution_checks(regression test_regressions.c
    unpriced-market execution-duplicate realised-multiplier invalid-fill cancelled-fill order-duplicate quote-rollback)
umicom_add_trading_execution_checks(price test_price_evidence.c
    buy-sell freshness identity prices limits-stops policy-overflow economic-order
    workspace-refresh workspace-expiry workspace-gates workspace-policy workspace-metadata)
umicom_add_trading_execution_checks(fill test_fill_atomicity.c
    store-bounds position-arithmetic order-arithmetic workspace-replay workspace-invalid workspace-overflow)
umicom_add_trading_execution_checks(view test_evidence_views.c approved denial empty)
umicom_add_trading_execution_checks(clock test_simulation_clock.c delay backwards overflow)
