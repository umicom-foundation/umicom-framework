#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomOrderAdmissionChecks.cmake
#
# PURPOSE:
#   Register order identity and fixed-window admission regressions without broker I/O.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(admission_case IN ITEMS order_storage_admission order_throttle_boundaries workspace_order_lookup
            watchlist_admission workspace_market_lifecycle)
        set(target "umicom-trading-${admission_case}-test")
        add_executable("${target}" "${CMAKE_CURRENT_LIST_DIR}/../tests/trading/test_${admission_case}.c")
        target_link_libraries("${target}" PRIVATE Umicom::trading)
        set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("${target}")
        umicom_apply_sanitizers("${target}")
        add_test(NAME "framework.trading.admission.${admission_case}" COMMAND "${target}")
        set_tests_properties("framework.trading.admission.${admission_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;trading;admission;memory-only")
    endforeach()
endif()
