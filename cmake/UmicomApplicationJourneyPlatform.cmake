#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationJourneyPlatform.cmake
#
# PURPOSE:
#   Attach executable product journeys, evidence and readiness reporting to
#   the canonical application target and validation closure.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(UMICOM_APPLICATION_JOURNEY_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET umicom_application)
    message(FATAL_ERROR "Application Journey Platform requires umicom_application")
endif()
set(UMICOM_APPLICATION_JOURNEY_MODULES
    journey catalogue audit evidence runtime report)
foreach(UMICOM_APPLICATION_JOURNEY_MODULE IN LISTS UMICOM_APPLICATION_JOURNEY_MODULES)
    target_sources(umicom_application PRIVATE
        "${UMICOM_APPLICATION_JOURNEY_ROOT}/src/application/journey/${UMICOM_APPLICATION_JOURNEY_MODULE}.c")
endforeach()
if(BUILD_TESTING)
    function(umicom_add_application_journey_test target test_name source)
        add_executable("${target}"
            "${UMICOM_APPLICATION_JOURNEY_ROOT}/tests/application_journey/${source}")
        target_include_directories("${target}" PRIVATE
            "${UMICOM_APPLICATION_JOURNEY_ROOT}/tests/application_journey")
        target_link_libraries("${target}" PRIVATE Umicom::application)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;application;journey;studio;trader;acceptance")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${target}")
        endif()
    endfunction()
    foreach(UMICOM_APPLICATION_JOURNEY_MODULE IN LISTS UMICOM_APPLICATION_JOURNEY_MODULES)
        string(REPLACE "_" "-" UMICOM_APPLICATION_JOURNEY_SUFFIX
            "${UMICOM_APPLICATION_JOURNEY_MODULE}")
        umicom_add_application_journey_test(
            "umicom-application-journey-${UMICOM_APPLICATION_JOURNEY_SUFFIX}-test"
            "framework.application_journey.${UMICOM_APPLICATION_JOURNEY_MODULE}"
            "test_${UMICOM_APPLICATION_JOURNEY_MODULE}.c")
    endforeach()
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-first-run-workspace-test"
        "framework.application_journey.studio.first-run-workspace"
        "scenarios/test_studio_first_run_workspace.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-edit-build-test-test"
        "framework.application_journey.studio.edit-build-test"
        "scenarios/test_studio_edit_build_test.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-debug-session-test"
        "framework.application_journey.studio.debug-session"
        "scenarios/test_studio_debug_session.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-source-control-review-test"
        "framework.application_journey.studio.source-control-review"
        "scenarios/test_studio_source_control_review.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-visual-designer-round-trip-test"
        "framework.application_journey.studio.visual-designer-round-trip"
        "scenarios/test_studio_visual_designer_round_trip.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-ai-assisted-change-test"
        "framework.application_journey.studio.ai-assisted-change"
        "scenarios/test_studio_ai_assisted_change.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-layout-recovery-test"
        "framework.application_journey.studio.layout-recovery"
        "scenarios/test_studio_layout_recovery.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-studio-package-release-test"
        "framework.application_journey.studio.package-release"
        "scenarios/test_studio_package_release.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-simulation-startup-test"
        "framework.application_journey.trader.simulation-startup"
        "scenarios/test_trader_simulation_startup.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-market-data-navigation-test"
        "framework.application_journey.trader.market-data-navigation"
        "scenarios/test_trader_market_data_navigation.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-simulation-order-test"
        "framework.application_journey.trader.simulation-order"
        "scenarios/test_trader_simulation_order.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-cancel-reconcile-test"
        "framework.application_journey.trader.cancel-reconcile"
        "scenarios/test_trader_cancel_reconcile.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-kill-switch-recovery-test"
        "framework.application_journey.trader.kill-switch-recovery"
        "scenarios/test_trader_kill_switch_recovery.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-replay-strategy-test"
        "framework.application_journey.trader.replay-strategy"
        "scenarios/test_trader_replay_strategy.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-paper-session-recovery-test"
        "framework.application_journey.trader.paper-session-recovery"
        "scenarios/test_trader_paper_session_recovery.c")
    umicom_add_application_journey_test(
        "umicom-application-journey-trader-live-release-gate-test"
        "framework.application_journey.trader.live-release-gate"
        "scenarios/test_trader_live_release_gate.c")
endif()
message(STATUS "Umicom executable Studio and Trader acceptance journeys enabled")

