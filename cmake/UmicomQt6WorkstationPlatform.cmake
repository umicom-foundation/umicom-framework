#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomQt6WorkstationPlatform.cmake
#
# PURPOSE:
#   Provide the optional Qt6 renderer for the canonical Framework UI/workstation
#   contracts while retaining all reusable application semantics in C23 Framework
#   libraries and keeping Qt/C++ isolated at the frontend adapter boundary.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

option(UMICOM_BUILD_QT6_ADAPTER
       "Build the optional Framework Qt6 workstation adapter"
       OFF)

if(NOT UMICOM_BUILD_QT6_ADAPTER)
    return()
endif()

if(NOT TARGET umicom_ui OR NOT TARGET umicom_ui_components)
    message(FATAL_ERROR "UmicomQt6WorkstationPlatform.cmake requires canonical umicom_ui and umicom_ui_components targets")
endif()

if(TARGET umicom_ui_qt6)
    return()
endif()

if(NOT CMAKE_CXX_COMPILER_LOADED)
    enable_language(CXX)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

set(UMICOM_QT6_WORKSTATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

add_library(umicom_ui_qt6 STATIC
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/adapter_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/application_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/shell_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workbench_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/desktop_shell_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workspace_profile_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/activity_bar_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/appearance_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/breadcrumb_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/clipboard_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/keybinding_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/editor_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/editor_theme_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/menu_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/toolbar_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/status_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/dialog_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/notification_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/explorer_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/quick_access_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/pane_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/document_view_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/action_dispatch_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/source_control_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/layout_browser_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/layout_preview_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/cross_application_panel_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/declarative_renderer_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_badge_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_picker_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_diagnostics_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_history_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_inspector_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_menu_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_status_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/context_toolbar_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/component_catalogue_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/component_factory_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/containers_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/controls_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/navigation_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/views_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/window_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/media_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/feedback_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/accessibility_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/theme_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/monitor_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/shell_header_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/panel_frame_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/tab_host_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/split_host_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/dock_overlay_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/tool_rail_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/workspace_strip_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/command_bar_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/status_strip_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/inspector_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/palette_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/asset_browser_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/timeline_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/transport_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/chart_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/data_grid_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/node_graph_qt6.cpp"
    "${UMICOM_QT6_WORKSTATION_ROOT}/adapters/qt6/workstation/media_canvas_qt6.cpp"
)
add_library(Umicom::ui_qt6 ALIAS umicom_ui_qt6)
set_target_properties(umicom_ui_qt6 PROPERTIES
    EXPORT_NAME ui_qt6
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)
target_compile_features(umicom_ui_qt6 PRIVATE cxx_std_20)
target_compile_definitions(umicom_ui_qt6 PRIVATE UMICOM_QT6_NATIVE=1)
target_include_directories(umicom_ui_qt6 PUBLIC
    $<BUILD_INTERFACE:${UMICOM_QT6_WORKSTATION_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(umicom_ui_qt6 PUBLIC
    Umicom::ui
    Umicom::ui_components
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

foreach(_umi_qt6_optional_target IN ITEMS
        Umicom::desktop Umicom::diagnostic_ui Umicom::terminal_ui Umicom::vcs_ui)
    if(TARGET ${_umi_qt6_optional_target})
        target_link_libraries(umicom_ui_qt6 PUBLIC ${_umi_qt6_optional_target})
    endif()
endforeach()

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_ui_qt6)
endif()
if(UMICOM_ENABLE_STRICT_WARNINGS)
    if(MSVC)
        target_compile_options(umicom_ui_qt6 PRIVATE /WX /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom_ui_qt6 PRIVATE -Werror)
    endif()
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_ui_qt6)
endif()

if(BUILD_TESTING)
    function(umicom_add_qt6_adapter_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_QT6_WORKSTATION_ROOT}/${source}")
        target_compile_features("${target}" PRIVATE cxx_std_20)
        target_link_libraries("${target}" PRIVATE Umicom::ui_qt6)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(UMICOM_ENABLE_STRICT_WARNINGS)
            if(MSVC)
                target_compile_options("${target}" PRIVATE /WX /permissive-)
            elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options("${target}" PRIVATE -Werror)
            endif()
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;qt6-adapter")
    endfunction()

    umicom_add_qt6_adapter_test(
        umicom-qt6-adapter-test
        framework.qt6.adapter
        tests/qt6/test_adapter.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-application-test
        framework.qt6.application
        tests/qt6/test_application.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-shell-test
        framework.qt6.shell
        tests/qt6/test_shell.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workbench-test
        framework.qt6.workbench
        tests/qt6/test_workbench.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-desktop-shell-test
        framework.qt6.desktop.shell
        tests/qt6/test_desktop_shell.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workspace-profile-test
        framework.qt6.workspace.profile
        tests/qt6/test_workspace_profile.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-activity-bar-test
        framework.qt6.activity.bar
        tests/qt6/test_activity_bar.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-appearance-test
        framework.qt6.appearance
        tests/qt6/test_appearance.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-breadcrumb-test
        framework.qt6.breadcrumb
        tests/qt6/test_breadcrumb.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-clipboard-test
        framework.qt6.clipboard
        tests/qt6/test_clipboard.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-keybinding-test
        framework.qt6.keybinding
        tests/qt6/test_keybinding.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-editor-test
        framework.qt6.editor
        tests/qt6/test_editor.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-editor-theme-test
        framework.qt6.editor.theme
        tests/qt6/test_editor_theme.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-menu-test
        framework.qt6.menu
        tests/qt6/test_menu.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-toolbar-test
        framework.qt6.toolbar
        tests/qt6/test_toolbar.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-status-test
        framework.qt6.status
        tests/qt6/test_status.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-dialog-test
        framework.qt6.dialog
        tests/qt6/test_dialog.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-notification-test
        framework.qt6.notification
        tests/qt6/test_notification.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-explorer-test
        framework.qt6.explorer
        tests/qt6/test_explorer.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-quick-access-test
        framework.qt6.quick.access
        tests/qt6/test_quick_access.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-pane-test
        framework.qt6.pane
        tests/qt6/test_pane.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-document-view-test
        framework.qt6.document.view
        tests/qt6/test_document_view.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-action-dispatch-test
        framework.qt6.action.dispatch
        tests/qt6/test_action_dispatch.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-source-control-test
        framework.qt6.source.control
        tests/qt6/test_source_control.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-layout-browser-test
        framework.qt6.layout.browser
        tests/qt6/test_layout_browser.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-layout-preview-test
        framework.qt6.layout.preview
        tests/qt6/test_layout_preview.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-cross-application-panel-test
        framework.qt6.cross.application.panel
        tests/qt6/test_cross_application_panel.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-declarative-renderer-test
        framework.qt6.declarative.renderer
        tests/qt6/test_declarative_renderer.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-badge-test
        framework.qt6.context.badge
        tests/qt6/test_context_badge.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-picker-test
        framework.qt6.context.picker
        tests/qt6/test_context_picker.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-diagnostics-test
        framework.qt6.context.diagnostics
        tests/qt6/test_context_diagnostics.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-history-test
        framework.qt6.context.history
        tests/qt6/test_context_history.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-inspector-test
        framework.qt6.context.inspector
        tests/qt6/test_context_inspector.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-menu-test
        framework.qt6.context.menu
        tests/qt6/test_context_menu.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-status-test
        framework.qt6.context.status
        tests/qt6/test_context_status.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-context-toolbar-test
        framework.qt6.context.toolbar
        tests/qt6/test_context_toolbar.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-component-catalogue-test
        framework.qt6.component.catalogue
        tests/qt6/test_component_catalogue.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-component-factory-test
        framework.qt6.component.factory
        tests/qt6/test_component_factory.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-containers-test
        framework.qt6.containers
        tests/qt6/test_containers.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-controls-test
        framework.qt6.controls
        tests/qt6/test_controls.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-navigation-test
        framework.qt6.navigation
        tests/qt6/test_navigation.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-views-test
        framework.qt6.views
        tests/qt6/test_views.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-window-test
        framework.qt6.window
        tests/qt6/test_window.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-media-test
        framework.qt6.media
        tests/qt6/test_media.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-feedback-test
        framework.qt6.feedback
        tests/qt6/test_feedback.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-accessibility-test
        framework.qt6.accessibility
        tests/qt6/test_accessibility.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-theme-test
        framework.qt6.theme
        tests/qt6/test_theme.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-monitor-test
        framework.qt6.monitor
        tests/qt6/test_monitor.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-shell-header-test
        framework.qt6.workstation.shell.header
        tests/qt6/test_workstation_shell_header.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-panel-frame-test
        framework.qt6.workstation.panel.frame
        tests/qt6/test_workstation_panel_frame.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-tab-host-test
        framework.qt6.workstation.tab.host
        tests/qt6/test_workstation_tab_host.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-split-host-test
        framework.qt6.workstation.split.host
        tests/qt6/test_workstation_split_host.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-dock-overlay-test
        framework.qt6.workstation.dock.overlay
        tests/qt6/test_workstation_dock_overlay.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-tool-rail-test
        framework.qt6.workstation.tool.rail
        tests/qt6/test_workstation_tool_rail.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-workspace-strip-test
        framework.qt6.workstation.workspace.strip
        tests/qt6/test_workstation_workspace_strip.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-command-bar-test
        framework.qt6.workstation.command.bar
        tests/qt6/test_workstation_command_bar.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-status-strip-test
        framework.qt6.workstation.status.strip
        tests/qt6/test_workstation_status_strip.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-inspector-test
        framework.qt6.workstation.inspector
        tests/qt6/test_workstation_inspector.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-palette-test
        framework.qt6.workstation.palette
        tests/qt6/test_workstation_palette.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-asset-browser-test
        framework.qt6.workstation.asset.browser
        tests/qt6/test_workstation_asset_browser.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-timeline-test
        framework.qt6.workstation.timeline
        tests/qt6/test_workstation_timeline.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-transport-test
        framework.qt6.workstation.transport
        tests/qt6/test_workstation_transport.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-chart-test
        framework.qt6.workstation.chart
        tests/qt6/test_workstation_chart.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-data-grid-test
        framework.qt6.workstation.data.grid
        tests/qt6/test_workstation_data_grid.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-node-graph-test
        framework.qt6.workstation.node.graph
        tests/qt6/test_workstation_node_graph.cpp
)
    umicom_add_qt6_adapter_test(
        umicom-qt6-workstation-media-canvas-test
        framework.qt6.workstation.media.canvas
        tests/qt6/test_workstation_media_canvas.cpp
)
endif()

install(TARGETS umicom_ui_qt6
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

message(STATUS "Umicom Qt6 workstation adapter enabled (Qt ${Qt6_VERSION})")
