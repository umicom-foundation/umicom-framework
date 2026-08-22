#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchContextHostPlatform.cmake
#
# PURPOSE:
#   Register the toolkit-neutral Workbench Context Host, persistence, policy,
#   projections, tests and optional GTK4 presentation integration.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)

set(UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(TARGET umicom_workbench_context_host)
    return()
endif()

if(NOT TARGET umicom_workbench_context_link)
    message(FATAL_ERROR
        "Workbench Context Host requires Umicom::workbench_context_link")
endif()
if(NOT TARGET umicom_ui)
    message(FATAL_ERROR
        "Workbench Context Host requires the canonical Umicom::ui target")
endif()
if(NOT TARGET umicom_platform)
    message(FATAL_ERROR
        "Workbench Context Host requires the canonical Umicom::platform target")
endif()

add_library(umicom_workbench_context_host STATIC
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/account_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/ai_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/application_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/audit.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/auto_link_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/command.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/compatibility.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/controller.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/delivery.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/delivery_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/diagnostic_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/diagnostics.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/dispatcher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/endpoint.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/endpoint_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/endpoint_registry.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/event.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/event_queue.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/focus_propagation.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/group_definition.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/group_picker_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/health.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/history_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/host.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/inbox.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/inbox_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/inbox_registry.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/inspector_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/instrument_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/media_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/metrics.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/metrics_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/observation.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/observer.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/payload.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/profile.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/profile_apply.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/profile_catalogue.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/project_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/publication_guard.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/query.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/route_preview.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/scope_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/search.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/selection_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/session.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/session_service.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/snapshot.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/source_control_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/source_location_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/status_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/terminal_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/test_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/throttle.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/toolbar_projection.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/trade_publisher.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/types.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/workbench_adapter.c"
    "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/src/workbench_context_host/workspace_publisher.c"
)
add_library(Umicom::workbench_context_host ALIAS
    umicom_workbench_context_host)
set_target_properties(umicom_workbench_context_host PROPERTIES
    EXPORT_NAME workbench_context_host
)

target_include_directories(umicom_workbench_context_host PUBLIC
    $<BUILD_INTERFACE:${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(umicom_workbench_context_host PUBLIC
    Umicom::workbench_context_link
    Umicom::ui
    Umicom::platform
)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_context_host)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_context_host)
endif()

if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_context_host
    )
endif()

if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_endpoint_list.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_group_picker.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_history.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_inbox.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_inspector.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_internal.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_panel_badge.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_status.c"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_host_strip.c"
    )
    target_link_libraries(umicom_ui_gtk4 PUBLIC
        Umicom::workbench_context_host
    )
endif()

install(
    TARGETS umicom_workbench_context_host
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/include/umicom/workbench_context_host"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)

install(
    FILES
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/resources/workbench-context-host-defaults.json"
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/resources/schemas/workbench-context-host.schema.json"
    DESTINATION
        "${CMAKE_INSTALL_DATADIR}/umicom/resources/workbench-context-host"
)

install(
    FILES
        "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/resources/themes/workbench-context-host.css"
    DESTINATION
        "${CMAKE_INSTALL_DATADIR}/umicom/resources/themes"
)

if(BUILD_TESTING)
    function(umicom_add_workbench_context_host_test target test_name source)
        add_executable("${target}"
            "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/${source}"
            "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/tests/workbench_context_host/test_support.c"
        )
        target_include_directories("${target}" PRIVATE
            "${UMICOM_WORKBENCH_CONTEXT_HOST_FRAMEWORK_ROOT}/tests/workbench_context_host"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_context_host
        )
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;workbench-context-host"
        )
    endfunction()

    umicom_add_workbench_context_host_test(umicom-workbench-context-host-account-publisher-test framework.workbench_context_host.account_publisher tests/workbench_context_host/test_account_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-ai-publisher-test framework.workbench_context_host.ai_publisher tests/workbench_context_host/test_ai_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-application-publisher-test framework.workbench_context_host.application_publisher tests/workbench_context_host/test_application_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-audit-test framework.workbench_context_host.audit tests/workbench_context_host/test_audit.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-auto-link-policy-test framework.workbench_context_host.auto_link_policy tests/workbench_context_host/test_auto_link_policy.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-command-test framework.workbench_context_host.command tests/workbench_context_host/test_command.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-compatibility-test framework.workbench_context_host.compatibility tests/workbench_context_host/test_compatibility.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-controller-test framework.workbench_context_host.controller tests/workbench_context_host/test_controller.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-cross-group-isolation-test framework.workbench_context_host.cross_group_isolation tests/workbench_context_host/test_cross_group_isolation.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-delivery-test framework.workbench_context_host.delivery tests/workbench_context_host/test_delivery.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-delivery-policy-test framework.workbench_context_host.delivery_policy tests/workbench_context_host/test_delivery_policy.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-diagnostic-publisher-test framework.workbench_context_host.diagnostic_publisher tests/workbench_context_host/test_diagnostic_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-diagnostics-test framework.workbench_context_host.diagnostics tests/workbench_context_host/test_diagnostics.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-dispatcher-test framework.workbench_context_host.dispatcher tests/workbench_context_host/test_dispatcher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-endpoint-test framework.workbench_context_host.endpoint tests/workbench_context_host/test_endpoint.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-endpoint-projection-test framework.workbench_context_host.endpoint_projection tests/workbench_context_host/test_endpoint_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-endpoint-reassignment-test framework.workbench_context_host.endpoint_reassignment tests/workbench_context_host/test_endpoint_reassignment.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-endpoint-registry-test framework.workbench_context_host.endpoint_registry tests/workbench_context_host/test_endpoint_registry.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-event-test framework.workbench_context_host.event tests/workbench_context_host/test_event.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-event-queue-test framework.workbench_context_host.event_queue tests/workbench_context_host/test_event_queue.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-focus-propagation-test framework.workbench_context_host.focus_propagation tests/workbench_context_host/test_focus_propagation.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-group-definition-test framework.workbench_context_host.group_definition tests/workbench_context_host/test_group_definition.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-group-picker-projection-test framework.workbench_context_host.group_picker_projection tests/workbench_context_host/test_group_picker_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-health-test framework.workbench_context_host.health tests/workbench_context_host/test_health.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-health-after-pressure-test framework.workbench_context_host.health_after_pressure tests/workbench_context_host/test_health_after_pressure.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-history-navigation-test framework.workbench_context_host.history_navigation tests/workbench_context_host/test_history_navigation.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-history-projection-test framework.workbench_context_host.history_projection tests/workbench_context_host/test_history_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-host-test framework.workbench_context_host.host tests/workbench_context_host/test_host.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-inbox-test framework.workbench_context_host.inbox tests/workbench_context_host/test_inbox.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-inbox-pressure-test framework.workbench_context_host.inbox_pressure tests/workbench_context_host/test_inbox_pressure.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-inbox-projection-test framework.workbench_context_host.inbox_projection tests/workbench_context_host/test_inbox_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-inbox-registry-test framework.workbench_context_host.inbox_registry tests/workbench_context_host/test_inbox_registry.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-inspector-projection-test framework.workbench_context_host.inspector_projection tests/workbench_context_host/test_inspector_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-instrument-publisher-test framework.workbench_context_host.instrument_publisher tests/workbench_context_host/test_instrument_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-media-publisher-test framework.workbench_context_host.media_publisher tests/workbench_context_host/test_media_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-metrics-test framework.workbench_context_host.metrics tests/workbench_context_host/test_metrics.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-metrics-projection-test framework.workbench_context_host.metrics_projection tests/workbench_context_host/test_metrics_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-observation-test framework.workbench_context_host.observation tests/workbench_context_host/test_observation.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-observation-deduplication-test framework.workbench_context_host.observation_deduplication tests/workbench_context_host/test_observation_deduplication.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-observer-test framework.workbench_context_host.observer tests/workbench_context_host/test_observer.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-payload-test framework.workbench_context_host.payload tests/workbench_context_host/test_payload.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-pin-across-group-switch-test framework.workbench_context_host.pin_across_group_switch tests/workbench_context_host/test_pin_across_group_switch.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-profile-test framework.workbench_context_host.profile tests/workbench_context_host/test_profile.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-profile-apply-test framework.workbench_context_host.profile_apply tests/workbench_context_host/test_profile_apply.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-profile-catalogue-test framework.workbench_context_host.profile_catalogue tests/workbench_context_host/test_profile_catalogue.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-project-publisher-test framework.workbench_context_host.project_publisher tests/workbench_context_host/test_project_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-publication-guard-test framework.workbench_context_host.publication_guard tests/workbench_context_host/test_publication_guard.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-publisher-permission-test framework.workbench_context_host.publisher_permission tests/workbench_context_host/test_publisher_permission.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-query-test framework.workbench_context_host.query tests/workbench_context_host/test_query.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-route-preview-test framework.workbench_context_host.route_preview tests/workbench_context_host/test_route_preview.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-route-preview-after-reassignment-test framework.workbench_context_host.route_preview_after_reassignment tests/workbench_context_host/test_route_preview_after_reassignment.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-scope-policy-test framework.workbench_context_host.scope_policy tests/workbench_context_host/test_scope_policy.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-search-test framework.workbench_context_host.search tests/workbench_context_host/test_search.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-selection-publisher-test framework.workbench_context_host.selection_publisher tests/workbench_context_host/test_selection_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-session-test framework.workbench_context_host.session tests/workbench_context_host/test_session.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-session-reassignment-test framework.workbench_context_host.session_reassignment tests/workbench_context_host/test_session_reassignment.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-session-service-test framework.workbench_context_host.session_service tests/workbench_context_host/test_session_service.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-snapshot-test framework.workbench_context_host.snapshot tests/workbench_context_host/test_snapshot.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-source-control-publisher-test framework.workbench_context_host.source_control_publisher tests/workbench_context_host/test_source_control_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-source-location-publisher-test framework.workbench_context_host.source_location_publisher tests/workbench_context_host/test_source_location_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-status-projection-test framework.workbench_context_host.status_projection tests/workbench_context_host/test_status_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-terminal-publisher-test framework.workbench_context_host.terminal_publisher tests/workbench_context_host/test_terminal_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-test-publisher-test framework.workbench_context_host.test_publisher tests/workbench_context_host/test_test_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-throttle-test framework.workbench_context_host.throttle tests/workbench_context_host/test_throttle.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-toolbar-projection-test framework.workbench_context_host.toolbar_projection tests/workbench_context_host/test_toolbar_projection.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-trade-publisher-test framework.workbench_context_host.trade_publisher tests/workbench_context_host/test_trade_publisher.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-types-test framework.workbench_context_host.types tests/workbench_context_host/test_types.c)
    umicom_add_workbench_context_host_test(umicom-workbench-context-host-workspace-publisher-test framework.workbench_context_host.workspace_publisher tests/workbench_context_host/test_workspace_publisher.c)
endif()

message(STATUS "Umicom workbench context host and application integration enabled")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchContextEventPlatform.cmake")
