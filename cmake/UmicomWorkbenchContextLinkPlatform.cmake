#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchContextLinkPlatform.cmake
#
# PURPOSE:
#   Register the Framework-owned context-linked workbench runtime, tests,
#   install/export rules and optional GTK4 adapter surfaces.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)

set(UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

if(TARGET umicom_workbench_context_link)
    return()
endif()

add_library(umicom_workbench_context_link STATIC
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/types.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/group_profile.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/panel_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/application_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/workspace_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/layout_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/monitor_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/publish_request.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/delivery_target.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/delivery_plan.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/route_hop.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/route_trace.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/active_context.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/context_cache_entry.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/history_entry.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/navigation_entry.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/pinned_context.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/recent_context.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/focus_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/activation_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/selection_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/source_location_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/instrument_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/account_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/trade_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/project_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/workspace_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/media_bridge.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/permission.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/access_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/audit_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/diagnostic.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/metric.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/conflict.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/sync_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/persistence_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/import_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/export_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/context_diff.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/inspector_row.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/group_picker_row.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/badge_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/panel_header_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/panel_host_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/toolbar_item.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/menu_item.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/status_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/accessibility_node.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/drag_drop_link.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/keyboard_navigation.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/search_result.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/filter.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/statistics.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/command.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/event.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/query.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/command_history.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/undo_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/session.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/workspace_link_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/application_link_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/multi_monitor_link_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/layout_link_model.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/panel_link_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/group_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/context_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/route_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/delivery_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/history_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/group_catalogue.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/binding_registry.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/active_context_store.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/history_store.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/pin_store.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/payload_builder.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/service.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/controller.c"
    "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/src/workbench_context_link/snapshot.c"
)
add_library(Umicom::workbench_context_link ALIAS umicom_workbench_context_link)
set_target_properties(umicom_workbench_context_link PROPERTIES
    EXPORT_NAME workbench_context_link
)

target_include_directories(umicom_workbench_context_link PUBLIC
    $<BUILD_INTERFACE:${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(umicom_workbench_context_link PUBLIC
    Umicom::context_channel
    Umicom::cross_application_panel
)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_context_link)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_context_link)
endif()

if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_context_link
    )
endif()

if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_group_picker.c"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_badge.c"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_panel_header.c"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_inspector.c"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_history.c"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_status.c"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_toolbar.c"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/adapters/gtk4/workbench_context_link_panel_host.c"
)
    target_link_libraries(umicom_ui_gtk4 PUBLIC
        Umicom::workbench_context_link
    )
endif()

install(
    TARGETS umicom_workbench_context_link
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/include/umicom/workbench_context_link"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)

install(
    FILES
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/resources/workbench-context-link-defaults.json"
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/resources/schemas/workbench-context-link.schema.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/workbench-context-link"
)
install(
    FILES
        "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/resources/themes/workbench-context-link.css"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/themes"
)

if(BUILD_TESTING)
    function(umicom_add_workbench_context_link_test target test_name source)
        add_executable("${target}"
            "${UMICOM_WORKBENCH_CONTEXT_LINK_FRAMEWORK_ROOT}/${source}"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_context_link
        )
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;workbench-context-link"
        )
    endfunction()

    umicom_add_workbench_context_link_test(umicom-workbench-context-link-group-profile-test framework.workbench_context_link.group_profile tests/workbench_context_link/test_group_profile.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-panel-binding-test framework.workbench_context_link.panel_binding tests/workbench_context_link/test_panel_binding.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-application-binding-test framework.workbench_context_link.application_binding tests/workbench_context_link/test_application_binding.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-workspace-binding-test framework.workbench_context_link.workspace_binding tests/workbench_context_link/test_workspace_binding.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-layout-binding-test framework.workbench_context_link.layout_binding tests/workbench_context_link/test_layout_binding.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-monitor-binding-test framework.workbench_context_link.monitor_binding tests/workbench_context_link/test_monitor_binding.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-publish-request-test framework.workbench_context_link.publish_request tests/workbench_context_link/test_publish_request.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-delivery-target-test framework.workbench_context_link.delivery_target tests/workbench_context_link/test_delivery_target.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-delivery-plan-test framework.workbench_context_link.delivery_plan tests/workbench_context_link/test_delivery_plan.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-route-hop-test framework.workbench_context_link.route_hop tests/workbench_context_link/test_route_hop.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-route-trace-test framework.workbench_context_link.route_trace tests/workbench_context_link/test_route_trace.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-active-context-test framework.workbench_context_link.active_context tests/workbench_context_link/test_active_context.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-context-cache-entry-test framework.workbench_context_link.context_cache_entry tests/workbench_context_link/test_context_cache_entry.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-history-entry-test framework.workbench_context_link.history_entry tests/workbench_context_link/test_history_entry.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-navigation-entry-test framework.workbench_context_link.navigation_entry tests/workbench_context_link/test_navigation_entry.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-pinned-context-test framework.workbench_context_link.pinned_context tests/workbench_context_link/test_pinned_context.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-recent-context-test framework.workbench_context_link.recent_context tests/workbench_context_link/test_recent_context.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-focus-policy-test framework.workbench_context_link.focus_policy tests/workbench_context_link/test_focus_policy.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-activation-policy-test framework.workbench_context_link.activation_policy tests/workbench_context_link/test_activation_policy.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-selection-bridge-test framework.workbench_context_link.selection_bridge tests/workbench_context_link/test_selection_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-source-location-bridge-test framework.workbench_context_link.source_location_bridge tests/workbench_context_link/test_source_location_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-instrument-bridge-test framework.workbench_context_link.instrument_bridge tests/workbench_context_link/test_instrument_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-account-bridge-test framework.workbench_context_link.account_bridge tests/workbench_context_link/test_account_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-trade-bridge-test framework.workbench_context_link.trade_bridge tests/workbench_context_link/test_trade_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-project-bridge-test framework.workbench_context_link.project_bridge tests/workbench_context_link/test_project_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-workspace-bridge-test framework.workbench_context_link.workspace_bridge tests/workbench_context_link/test_workspace_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-media-bridge-test framework.workbench_context_link.media_bridge tests/workbench_context_link/test_media_bridge.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-permission-test framework.workbench_context_link.permission tests/workbench_context_link/test_permission.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-access-policy-test framework.workbench_context_link.access_policy tests/workbench_context_link/test_access_policy.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-audit-record-test framework.workbench_context_link.audit_record tests/workbench_context_link/test_audit_record.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-diagnostic-test framework.workbench_context_link.diagnostic tests/workbench_context_link/test_diagnostic.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-metric-test framework.workbench_context_link.metric tests/workbench_context_link/test_metric.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-conflict-test framework.workbench_context_link.conflict tests/workbench_context_link/test_conflict.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-sync-state-test framework.workbench_context_link.sync_state tests/workbench_context_link/test_sync_state.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-persistence-record-test framework.workbench_context_link.persistence_record tests/workbench_context_link/test_persistence_record.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-import-record-test framework.workbench_context_link.import_record tests/workbench_context_link/test_import_record.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-export-record-test framework.workbench_context_link.export_record tests/workbench_context_link/test_export_record.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-context-diff-test framework.workbench_context_link.context_diff tests/workbench_context_link/test_context_diff.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-inspector-row-test framework.workbench_context_link.inspector_row tests/workbench_context_link/test_inspector_row.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-group-picker-row-test framework.workbench_context_link.group_picker_row tests/workbench_context_link/test_group_picker_row.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-badge-model-test framework.workbench_context_link.badge_model tests/workbench_context_link/test_badge_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-panel-header-model-test framework.workbench_context_link.panel_header_model tests/workbench_context_link/test_panel_header_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-panel-host-model-test framework.workbench_context_link.panel_host_model tests/workbench_context_link/test_panel_host_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-toolbar-item-test framework.workbench_context_link.toolbar_item tests/workbench_context_link/test_toolbar_item.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-menu-item-test framework.workbench_context_link.menu_item tests/workbench_context_link/test_menu_item.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-status-model-test framework.workbench_context_link.status_model tests/workbench_context_link/test_status_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-accessibility-node-test framework.workbench_context_link.accessibility_node tests/workbench_context_link/test_accessibility_node.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-drag-drop-link-test framework.workbench_context_link.drag_drop_link tests/workbench_context_link/test_drag_drop_link.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-keyboard-navigation-test framework.workbench_context_link.keyboard_navigation tests/workbench_context_link/test_keyboard_navigation.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-search-result-test framework.workbench_context_link.search_result tests/workbench_context_link/test_search_result.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-filter-test framework.workbench_context_link.filter tests/workbench_context_link/test_filter.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-statistics-test framework.workbench_context_link.statistics tests/workbench_context_link/test_statistics.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-command-test framework.workbench_context_link.command tests/workbench_context_link/test_command.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-event-test framework.workbench_context_link.event tests/workbench_context_link/test_event.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-query-test framework.workbench_context_link.query tests/workbench_context_link/test_query.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-command-history-test framework.workbench_context_link.command_history tests/workbench_context_link/test_command_history.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-undo-record-test framework.workbench_context_link.undo_record tests/workbench_context_link/test_undo_record.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-session-test framework.workbench_context_link.session tests/workbench_context_link/test_session.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-workspace-link-model-test framework.workbench_context_link.workspace_link_model tests/workbench_context_link/test_workspace_link_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-application-link-model-test framework.workbench_context_link.application_link_model tests/workbench_context_link/test_application_link_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-multi-monitor-link-model-test framework.workbench_context_link.multi_monitor_link_model tests/workbench_context_link/test_multi_monitor_link_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-layout-link-model-test framework.workbench_context_link.layout_link_model tests/workbench_context_link/test_layout_link_model.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-panel-link-state-test framework.workbench_context_link.panel_link_state tests/workbench_context_link/test_panel_link_state.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-group-state-test framework.workbench_context_link.group_state tests/workbench_context_link/test_group_state.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-context-state-test framework.workbench_context_link.context_state tests/workbench_context_link/test_context_state.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-route-state-test framework.workbench_context_link.route_state tests/workbench_context_link/test_route_state.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-delivery-state-test framework.workbench_context_link.delivery_state tests/workbench_context_link/test_delivery_state.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-history-state-test framework.workbench_context_link.history_state tests/workbench_context_link/test_history_state.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-types-test framework.workbench_context_link.types tests/workbench_context_link/test_types.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-group-catalogue-test framework.workbench_context_link.group_catalogue tests/workbench_context_link/test_group_catalogue.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-binding-registry-test framework.workbench_context_link.binding_registry tests/workbench_context_link/test_binding_registry.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-service-routing-test framework.workbench_context_link.service_routing tests/workbench_context_link/test_service_routing.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-service-rejection-test framework.workbench_context_link.service_rejection tests/workbench_context_link/test_service_rejection.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-service-pin-test framework.workbench_context_link.service_pin tests/workbench_context_link/test_service_pin.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-snapshot-test framework.workbench_context_link.snapshot tests/workbench_context_link/test_snapshot.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-controller-test framework.workbench_context_link.controller tests/workbench_context_link/test_controller.c)
    umicom_add_workbench_context_link_test(umicom-workbench-context-link-payload-builder-test framework.workbench_context_link.payload_builder tests/workbench_context_link/test_payload_builder.c)

endif()

message(STATUS "Umicom context-linked workbench runtime enabled")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchContextHostPlatform.cmake")
