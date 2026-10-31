#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBrokerWelcomeGtk4.cmake
#
# PURPOSE:
#   Build the common broker welcome surface and native ownership regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomIbkrGateway.cmake")
target_sources(umicom_ibkr_connection_gtk4 PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/broker_welcome.c"
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/broker_workspace.c")
target_compile_definitions(umicom_ibkr_connection_gtk4 PUBLIC
    UMICOM_HAS_BROKER_WELCOME_GTK4=1 UMICOM_HAS_IBKR_CONNECTION_GTK4=1
    UMICOM_HAS_BROKER_WORKSPACE_GTK4=1
    UMICOM_HAS_BROKER_WORKSPACE_LAYOUT_GTK4=1)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/welcome_gtk4.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_gtk4.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/workspace_gtk4.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/welcome_gtk4.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/access_gtk4.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/workspace_gtk4.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
if(BUILD_TESTING)
    add_executable(umicom-broker-welcome-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_broker_welcome.c")
    target_link_libraries(umicom-broker-welcome-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-welcome-test)
    foreach(case modes gateway retained reentrant close-callback close-notify failure optional-profiles account-tabs retained-tabs
        session-layout session-titles session-retired session-close-guide session-close-stage session-reentry-begin session-reentry-back
        session-paper session-live session-gateway session-back session-guest)
        add_test(NAME framework.broker_welcome.${case}
            COMMAND umicom-broker-welcome-test ${case})
        set_tests_properties(framework.broker_welcome.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;startup;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-welcome-test)
    endif()
endif()

# Native plan projection must preserve the same mode as the headless service.
if(BUILD_TESTING)
    add_executable(umicom-broker-access-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_access_native.c")
    target_link_libraries(umicom-broker-access-native-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-access-native-test)
    foreach(case defaults custom invalid consent-reopen launcher-copy parent-retired)
        add_test(NAME framework.broker_access_native.${case}
            COMMAND umicom-broker-access-native-test ${case})
        set_tests_properties(framework.broker_access_native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;startup;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-access-native-test)
    endif()
endif()

# Exercise asynchronous settings with isolated fixture paths, never user profiles.
if(BUILD_TESTING)
    add_executable(umicom-broker-profile-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_profile_native.c")
    target_link_libraries(umicom-broker-profile-native-test PRIVATE
        Umicom::ibkr_connection_gtk4 Umicom::platform)
    umicom_ibkr_target(umicom-broker-profile-native-test)
    foreach(case save-load missing damaged endpoint-ack stale-fields retired collapsed-settings)
        set(fixture "${CMAKE_CURRENT_BINARY_DIR}/broker-profile-native-fixtures/${case}")
        file(MAKE_DIRECTORY "${fixture}")
        add_test(NAME framework.broker_profile_native.${case}
            COMMAND umicom-broker-profile-native-test ${case} "${fixture}")
        set_tests_properties(framework.broker_profile_native.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;settings;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-profile-native-test)
    endif()
endif()

# The live controls and projection tests are separate: lifecycle checks use the
# real inspector without connecting, while synthetic observations exercise the
# workspace renderer without any network endpoint or personal account.
if(BUILD_TESTING)
    add_executable(umicom-broker-workspace-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_broker_workspace.c")
    target_link_libraries(umicom-broker-workspace-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-workspace-test)
    foreach(case invalid reuse reopen parent-retired retained-controls quote-guards quote-mode-guards search-guards order-guards execution-guards history-guards depth-guards pnl-guards scanner-guards completed-guards details-guards options-guards tape-guards)
        add_test(NAME framework.broker_workspace.${case}
            COMMAND umicom-broker-workspace-test ${case})
        set_tests_properties(framework.broker_workspace.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;gtk4;ownership;regression")
    endforeach()
    add_executable(umicom-broker-workspace-projection-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_workspace_projection.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/broker_workspace.c")
    target_link_libraries(umicom-broker-workspace-projection-test PRIVATE
        Umicom::ibkr_connection PkgConfig::IBKR_GTK4)
    # Match the production chart features while keeping all broker observations synthetic.
    if(TARGET Umicom::ibkr_historical_chart)
        target_link_libraries(umicom-broker-workspace-projection-test PRIVATE Umicom::ibkr_historical_chart)
        target_compile_definitions(umicom-broker-workspace-projection-test PRIVATE UMI_IBKR_HAS_HISTORY_MODEL=1)
    endif()
    if(TARGET Umicom::chart_cairo AND TARGET Umicom::ibkr_historical_chart)
        target_link_libraries(umicom-broker-workspace-projection-test PRIVATE Umicom::chart_cairo)
        target_compile_definitions(umicom-broker-workspace-projection-test PRIVATE UMI_IBKR_HAS_HISTORY_CHART=1)
    endif()
    umicom_ibkr_target(umicom-broker-workspace-projection-test)
    foreach(case selection reentrant decimal completion reconnect guidance utf8
        risk-empty risk-values risk-unavailable risk-stale risk-delayed risk-legacy risk-unknown-basis risk-independent risk-reconnect risk-reentrant
        table-transport table-clock table-message table-stale table-values table-positions table-summary-complete table-positions-complete table-clear-values table-clear-positions table-account table-generation table-unused table-selection table-reconnect table-unchanged
        quote-selection quote-invalid quote-values quote-age quote-cancel-failure quote-reconnect quote-reentrant quote-retained
        quote-mode-frozen quote-mode-delayed quote-mode-delayed-frozen quote-mode-change quote-mode-invalid
        search-select search-stale search-replaced search-active-quote search-abandon
        search-reconnect search-utf8 search-reentrant search-retained search-rejected search-empty
        orders-request orders-all orders-rows orders-empty orders-stale orders-reconnect
        orders-rejected orders-reentrant orders-retained orders-utf8 orders-external-request orders-manual
        trades-select trades-rows trades-late-fees trades-conflicting-fees trades-empty trades-partial
        trades-abandon trades-changed-request trades-external-request trades-stale trades-reconnect
        trades-retained trades-reentrant trades-utf8 trades-rejected trades-account-change
        history-request history-invalid history-pacing history-range history-cancel history-cancel-failure history-request-replaced history-external-request history-empty history-stale history-reconnect history-retained history-reentrant history-query-identity history-utf8 history-volume-unavailable history-render-unavailable history-timeout history-range-clamp
        depth-retained depth-external-request depth-invalid depth-mismatch depth-rejected depth-smart depth-stop depth-stop-failure depth-request-replaced depth-one-sided depth-row-stale depth-stale depth-failed depth-reentrant depth-reset depth-reconnect depth-query-identity depth-request depth-utf8
        pnl-retained pnl-no-account pnl-external-request pnl-invalid pnl-rejected pnl-position pnl-stop pnl-stop-failure pnl-request-replaced pnl-missing pnl-stale pnl-failed pnl-utf8 pnl-reentrant pnl-reconnect pnl-scope-identity pnl-account
        scanner-results scanner-invalid scanner-rejected scanner-external-request scanner-stop scanner-stop-failure scanner-request-replaced scanner-refresh-replaced scanner-row-copy-failed scanner-stale scanner-failed scanner-empty scanner-utf8 scanner-reentrant scanner-model-retired scanner-retained scanner-query-identity scanner-reconnect scanner-quote-busy
        completed-rows completed-all completed-external completed-rejected completed-empty completed-partial completed-stale completed-row-stale completed-copy-failure completed-reconnect completed-retained completed-reentrant completed-utf8 completed-account-scope completed-scope-intent completed-failed
        details-rows details-invalid details-multiple details-partial details-empty details-failed details-stale details-copy-failure details-reconnect details-retained details-reentrant details-utf8 details-rejected details-external details-abandon details-replaced-abandon details-intent details-quote details-option details-disconnect
        options-retained options-invalid options-rejected options-external options-intent options-abandon options-replaced-abandon options-partial options-empty options-stale options-failed options-copy-failure options-item-failure options-utf8 options-reentrant options-model-retired options-put options-call options-empty-route options-replaced-selection options-disconnect options-reconnect options-selection-intent options-long-symbol options-long-filter options-long-currency options-long-route
        tape-retained tape-invalid tape-kind-invalid tape-ignore-invalid tape-bid-ask tape-midpoint tape-all-last tape-rejected tape-pacing tape-session-stale tape-external tape-intent tape-empty tape-row-stale tape-stale tape-failed tape-copy-failure tape-capacity tape-utf8 tape-reentrant tape-cell-retired tape-disconnect tape-reconnect tape-stop tape-stop-failure tape-replaced-stop tape-rows
        stream-retained stream-invalid stream-kind-invalid stream-midpoint stream-bid stream-ask stream-all-hours stream-rejected stream-pacing stream-session-stale stream-external stream-intent stream-empty stream-stale stream-failed stream-copy-failure stream-capacity stream-volume-missing stream-render-failure stream-utf8 stream-reentrant stream-cache stream-rollover stream-range stream-range-clamp stream-modes stream-disconnect stream-reconnect stream-stop stream-stop-failure stream-replaced-stop)
        add_test(NAME framework.broker_workspace_projection.${case}
            COMMAND umicom-broker-workspace-projection-test ${case})
        set_tests_properties(framework.broker_workspace_projection.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;gtk4;observation;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-workspace-test)
        umicom_register_validation_target(umicom-broker-workspace-projection-test)
    endif()
endif()

# Construction must be atomic to host callbacks, without opening broker sockets.
if(BUILD_TESTING)
    add_executable(umicom-broker-construction-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_connection_construction.c")
    target_link_libraries(umicom-broker-construction-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-construction-test)
    foreach(case default paper live custom unregistered-parent nonwindow-parent parent-without-app
        prepared callback-child after-child callback-parent callback-retained callback-detach
        callback-replace callback-plan callback-command workspace-reentrant workspace-destroy workspace-parent)
        add_test(NAME framework.broker_construction.${case}
            COMMAND umicom-broker-construction-test ${case})
        set_tests_properties(framework.broker_construction.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-construction-test)
    endif()
endif()

# Exercise real page ownership without connecting to a broker.
if(BUILD_TESTING)
    add_executable(umicom-broker-workspace-layout-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_workspace_layout.c")
    target_link_libraries(umicom-broker-workspace-layout-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-workspace-layout-test)
    foreach(case IN ITEMS names
        defaults
        show-chart
        show-after-reorder
        move-account
        move-chart
        move-all
        restore
        restore-reordered
        restore-snapshot
        split
        toolbar-show
        toolbar-move
        toolbar-split
        retained
        invalid-page
        invalid-pane
        invalid-orientation
        null-output
        invalid-duplicate
        invalid-selection
        invalid-empty-selection
        invalid-divider
        invalid-enum
        invalid-order
        invalid-plan-null
        move-close
        apply-close
        select-close
        split-close
        move-reenter
        apply-reenter
        move-release
        apply-release)
        add_test(NAME framework.broker_workspace_layout.${case}
            COMMAND umicom-broker-workspace-layout-test ${case})
        set_tests_properties(framework.broker_workspace_layout.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;broker;layout;gtk4;ownership;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-workspace-layout-test)
    endif()
endif()

# Order-ticket and watchlist interaction checks use the real widgets without
# opening a broker connection. The owner runs these with native qualification.
if(BUILD_TESTING)
    add_executable(umicom-broker-ticket-controls-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_ticket_controls.c")
    target_link_libraries(umicom-broker-ticket-controls-test PRIVATE Umicom::ibkr_connection_gtk4)
    umicom_ibkr_target(umicom-broker-ticket-controls-test)
    foreach(_case IN ITEMS ticket-default pages watchlist watchlist-roundtrip watchlist-malformed watchlist-oversized retired)
        add_test(NAME framework.broker_ticket_controls.${_case}
            COMMAND umicom-broker-ticket-controls-test ${_case})
        set_tests_properties(framework.broker_ticket_controls.${_case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;broker;gtk4;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-broker-ticket-controls-test)
    endif()
endif()
