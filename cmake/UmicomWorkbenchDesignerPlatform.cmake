#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchDesignerPlatform.cmake
#
# PURPOSE:
#   Register the Framework-owned Layout Browser, visual layout designer,
#   toolkit-neutral design models, GTK4 adapter and focused regression tests.
#
# ARCHITECTURE:
#   Semantic layout state remains owned by Umicom Framework. The Data Server
#   remains authoritative for user layouts and sessions. Frontend adapters
#   render immutable models and submit typed commands through the Layout
#   Designer Slave Controller; they do not own an independent layout model.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(TARGET umicom_workbench_designer)
    return()
endif()

if(NOT TARGET Umicom::workbench_layout OR
   NOT TARGET Umicom::workbench_layout_data)
    message(FATAL_ERROR
        "The workbench designer requires the semantic layout and Data Server "
        "layout persistence targets.")
endif()

add_library(umicom_workbench_designer STATIC
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/access.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/alignment.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/alignment_guides.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/autosave.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/breadcrumbs.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/browser.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/browser_actions.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/browser_preview.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/browser_query.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/canvas.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/canvas_accessibility.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/clipboard.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/collaboration.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/collaboration_cursor.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/command.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/command_context.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/command_history.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/command_palette.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/commands.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/controller.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/diagnostics.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/diff_view.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/docking_guides.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/drag.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/drag_operation.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/drop_target.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/events.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/floating_editor.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/focus_ring.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/grid.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/import_export.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/inspector_validation.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/keymap.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/layout_factory.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/layout_statistics.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/lease_view.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/lock.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/marquee.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/merge_view.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/metrics.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/minimap.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/model_bundle.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/monitor_canvas.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/multi_transform.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/outline_filter.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/palette.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/panel_contribution.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/persistence_bridge.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/preferences.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/presence.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/preview.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/properties.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/property_groups.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/recent.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/recovery_bridge.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/resize.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/responsive_preview.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/review.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/ruler.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/save_guard.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/selection.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/service.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/session.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/snap.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/snapshot.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/split_editor.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/status.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/tab_editor.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/toolbar.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/tree.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/types.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/validation_gate.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/viewport.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/viewport_history.c"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer/workspace.c"
)
add_library(Umicom::workbench_designer ALIAS umicom_workbench_designer)
set_target_properties(umicom_workbench_designer PROPERTIES
    EXPORT_NAME workbench_designer
)

target_include_directories(umicom_workbench_designer
    PUBLIC
        $<BUILD_INTERFACE:${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/src/workbench_designer"
)
target_link_libraries(umicom_workbench_designer
    PUBLIC
        Umicom::workbench_layout
        Umicom::workbench_layout_data
)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_designer)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_designer)
endif()

# The aggregate Framework target exposes the visual designer to first-party
# applications while bounded consumers may link this target directly.
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_designer
    )
endif()

# GTK4 remains an optional adapter. No GTK type enters the public semantic
# designer model or persistent layout document.
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/layout_browser_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/layout_browser_preview_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_canvas_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_command_palette_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_docking_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_minimap_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_palette_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_properties_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_review_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_status_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_toolbar_gtk4.c"
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4/workbench_designer_tree_gtk4.c"
    )
    target_include_directories(umicom_ui_gtk4 PRIVATE
        "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/adapters/gtk4"
    )
    target_link_libraries(umicom_ui_gtk4 PUBLIC
        Umicom::workbench_designer
    )
endif()

if(BUILD_TESTING)
    function(umicom_add_workbench_designer_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable(
            "${target}"
            "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/${source}"
        )
        target_include_directories("${target}" PRIVATE
            "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/tests/workbench_designer"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_designer
        )
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_add_workbench_designer_test(
        umicom-workbench-designer-access-policy-test
        framework.workbench_designer.access_policy
        tests/workbench_designer/test_access_policy.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-alignment-distribution-test
        framework.workbench_designer.alignment_distribution
        tests/workbench_designer/test_alignment_distribution.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-alignment-guides-test
        framework.workbench_designer.alignment_guides
        tests/workbench_designer/test_alignment_guides.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-autosave-save-guard-test
        framework.workbench_designer.autosave_save_guard
        tests/workbench_designer/test_autosave_save_guard.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-browser-query-actions-test
        framework.workbench_designer.browser_query_actions
        tests/workbench_designer/test_browser_query_actions.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-canvas-accessibility-test
        framework.workbench_designer.canvas_accessibility
        tests/workbench_designer/test_canvas_accessibility.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-canvas-tree-test
        framework.workbench_designer.canvas_tree
        tests/workbench_designer/test_canvas_tree.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-capacity-boundaries-test
        framework.workbench_designer.capacity_boundaries
        tests/workbench_designer/test_capacity_boundaries.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-clipboard-test
        framework.workbench_designer.clipboard
        tests/workbench_designer/test_clipboard.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-collaboration-cursor-test
        framework.workbench_designer.collaboration_cursor
        tests/workbench_designer/test_collaboration_cursor.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-collaboration-presence-lease-test
        framework.workbench_designer.collaboration_presence_lease
        tests/workbench_designer/test_collaboration_presence_lease.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-command-context-test
        framework.workbench_designer.command_context
        tests/workbench_designer/test_command_context.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-command-history-test
        framework.workbench_designer.command_history
        tests/workbench_designer/test_command_history.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-command-palette-property-groups-test
        framework.workbench_designer.command_palette_property_groups
        tests/workbench_designer/test_command_palette_property_groups.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-diff-merge-review-test
        framework.workbench_designer.diff_merge_review
        tests/workbench_designer/test_diff_merge_review.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-drag-drop-guides-test
        framework.workbench_designer.drag_drop_guides
        tests/workbench_designer/test_drag_drop_guides.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-end-to-end-coding-test
        framework.workbench_designer.end_to_end_coding
        tests/workbench_designer/test_end_to_end_coding.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-end-to-end-trading-test
        framework.workbench_designer.end_to_end_trading
        tests/workbench_designer/test_end_to_end_trading.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-events-panel-workspace-test
        framework.workbench_designer.events_panel_workspace
        tests/workbench_designer/test_events_panel_workspace.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-focus-ring-test
        framework.workbench_designer.focus_ring
        tests/workbench_designer/test_focus_ring.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-inspector-validation-test
        framework.workbench_designer.inspector_validation
        tests/workbench_designer/test_inspector_validation.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-keymap-toolbar-status-test
        framework.workbench_designer.keymap_toolbar_status
        tests/workbench_designer/test_keymap_toolbar_status.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-layout-factory-breadcrumbs-test
        framework.workbench_designer.layout_factory_breadcrumbs
        tests/workbench_designer/test_layout_factory_breadcrumbs.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-layout-statistics-test
        framework.workbench_designer.layout_statistics
        tests/workbench_designer/test_layout_statistics.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-metrics-test
        framework.workbench_designer.metrics
        tests/workbench_designer/test_metrics.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-model-bundle-test
        framework.workbench_designer.model_bundle
        tests/workbench_designer/test_model_bundle.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-monitor-responsive-test
        framework.workbench_designer.monitor_responsive
        tests/workbench_designer/test_monitor_responsive.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-multi-transform-test
        framework.workbench_designer.multi_transform
        tests/workbench_designer/test_multi_transform.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-outline-filter-test
        framework.workbench_designer.outline_filter
        tests/workbench_designer/test_outline_filter.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-palette-properties-test
        framework.workbench_designer.palette_properties
        tests/workbench_designer/test_palette_properties.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-persistence-bridge-test
        framework.workbench_designer.persistence_bridge
        tests/workbench_designer/test_persistence_bridge.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-preferences-test
        framework.workbench_designer.preferences
        tests/workbench_designer/test_preferences.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-preview-minimap-ruler-test
        framework.workbench_designer.preview_minimap_ruler
        tests/workbench_designer/test_preview_minimap_ruler.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-recovery-bridge-test
        framework.workbench_designer.recovery_bridge
        tests/workbench_designer/test_recovery_bridge.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-resize-marquee-test
        framework.workbench_designer.resize_marquee
        tests/workbench_designer/test_resize_marquee.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-selection-viewport-test
        framework.workbench_designer.selection_viewport
        tests/workbench_designer/test_selection_viewport.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-service-controller-snapshot-test
        framework.workbench_designer.service_controller_snapshot
        tests/workbench_designer/test_service_controller_snapshot.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-structural-editors-test
        framework.workbench_designer.structural_editors
        tests/workbench_designer/test_structural_editors.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-transfer-lock-recent-test
        framework.workbench_designer.transfer_lock_recent
        tests/workbench_designer/test_transfer_lock_recent.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-types-geometry-test
        framework.workbench_designer.types_geometry
        tests/workbench_designer/test_types_geometry.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-validation-gate-diagnostics-test
        framework.workbench_designer.validation_gate_diagnostics
        tests/workbench_designer/test_validation_gate_diagnostics.c
    )
    umicom_add_workbench_designer_test(
        umicom-workbench-designer-viewport-history-test
        framework.workbench_designer.viewport_history
        tests/workbench_designer/test_viewport_history.c
    )
endif()

install(TARGETS umicom_workbench_designer
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/include/umicom/workbench_designer"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)
install(FILES
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/include/umicom/ui/workbench_designer_gtk4.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui"
)
install(FILES
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/resources/workbench-designer-defaults.json"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/resources/workbench-designer-keymap.json"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/resources/workbench-designer-palette.json"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/resources/workbench-designer-contributions.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources"
)
install(FILES
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/resources/schemas/workbench-designer-defaults.schema.json"
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/resources/schemas/workbench-designer-palette.schema.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/schemas"
)
install(FILES
    "${UMICOM_WORKBENCH_DESIGNER_FRAMEWORK_ROOT}/resources/themes/workbench-designer.css"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/themes"
)

message(STATUS
    "Umicom Layout Browser and visual workbench designer enabled")
