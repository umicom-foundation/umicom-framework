#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTradingWorkstationInteractionPlatform.cmake
#
# PURPOSE:
#   Extend the existing trading UI and GTK4 adapter targets with guarded
#   workstation interaction, deterministic simulation and reusable native
#   trading panels. No product-owned trading engine or second UI target exists.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_TRADING_WORKSTATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(umicom_trading_workstation_attach)
    if(TARGET umicom_trading_ui)
        get_target_property(
            _umicom_trading_interaction_attached
            umicom_trading_ui
            UMICOM_TRADING_INTERACTION_ATTACHED)
        if(NOT _umicom_trading_interaction_attached)
            target_sources(umicom_trading_ui PRIVATE
                "${UMICOM_TRADING_WORKSTATION_ROOT}/src/trading_ui/actions.c"
                "${UMICOM_TRADING_WORKSTATION_ROOT}/src/trading_ui/action_controller.c"
                "${UMICOM_TRADING_WORKSTATION_ROOT}/src/trading_ui/simulation_market.c"
            )
            set_property(
                TARGET umicom_trading_ui
                PROPERTY UMICOM_TRADING_INTERACTION_ATTACHED TRUE)
        endif()
    endif()

    if(TARGET umicom_ui_gtk4 AND TARGET Umicom::trading_ui)
        get_target_property(
            _umicom_trading_gtk4_attached
            umicom_ui_gtk4
            UMICOM_TRADING_WORKSTATION_GTK4_ATTACHED)
        if(NOT _umicom_trading_gtk4_attached)
            target_sources(umicom_ui_gtk4 PRIVATE
                "${UMICOM_TRADING_WORKSTATION_ROOT}/adapters/gtk4/trading_ui/trading_panels_gtk4.c"
                "${UMICOM_TRADING_WORKSTATION_ROOT}/adapters/gtk4/trading_ui/trading_suite_workstation_gtk4.c"
            )
            target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::trading_ui)
            set_property(
                TARGET umicom_ui_gtk4
                PROPERTY UMICOM_TRADING_WORKSTATION_GTK4_ATTACHED TRUE)
        endif()
    endif()

    if(BUILD_TESTING AND TARGET Umicom::trading_ui AND
       NOT TARGET umicom-trading-ui-interaction-tests)
        add_executable(umicom-trading-ui-interaction-tests
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_main.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_support.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_actions.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_refresh.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_environment.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_filter.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_selection.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_draft.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_preview.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_kill_switch.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_simulation_seed.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_simulation_step.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_dispatch.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_order_filter.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_submit_gate.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_live_opt_in.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_controller_unknown_action.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_simulation_idempotence.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_simulation_environment_gate.c"
            "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/trading_ui_interaction/test_simulation_filter_preservation.c"
        )
        target_link_libraries(umicom-trading-ui-interaction-tests PRIVATE
            Umicom::trading_ui)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-trading-ui-interaction-tests)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-trading-ui-interaction-tests)
        endif()
        add_test(
            NAME framework.trading_ui.interaction
            COMMAND umicom-trading-ui-interaction-tests)
        set_tests_properties(framework.trading_ui.interaction PROPERTIES
            LABELS "framework;trading;ui;interaction;simulation;trader")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-trading-ui-interaction-tests)
        endif()
    endif()
endfunction()

umicom_trading_workstation_attach()
if(COMMAND cmake_language)
    cmake_language(DEFER CALL umicom_trading_workstation_attach)
endif()
