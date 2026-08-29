#-----------------------------------------------------------------------------
# Umicom Framework
# File: CMakeLists.txt
#
# PURPOSE:
#   Build the reusable C23 Umicom Framework as either:
#     1. a standalone repository; or
#     2. a subdirectory inside Umicom Studio or another Umicom application.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

project(
    UmicomFramework
    VERSION 0.9.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(UMICOM_ENABLE_STRICT_WARNINGS
       "Enable strict compiler warnings"
       ON)
option(UMICOM_ENABLE_SANITIZERS
       "Enable AddressSanitizer and UndefinedBehaviorSanitizer where supported"
       OFF)
option(UMICOM_BUILD_NATIVE_TOOL
       "Build the native umicom environment, build and repository command"
       ON)
option(UMICOM_BUILD_GTK4_ADAPTER
       "Build the optional Framework GTK4 reference UI adapter"
       OFF)
option(UMICOM_EDITOR_ENABLE_ASSEMBLY
       "Enable audited architecture-specific editor text scanners"
       ON)
option(UMICOM_DEVELOPER_ENABLE_ASSEMBLY
       "Enable audited developer-runtime processor hints"
       ON)
option(UMICOM_TERMINAL_ENABLE_ASSEMBLY
       "Enable audited terminal transcript scanners"
       ON)

# When Framework is configured by itself, place generated programs and
# libraries in predictable bin/ and lib/ folders.  When Framework is included
# by a parent application, the parent may already have selected these folders.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(Threads REQUIRED)

# The parent Umicom application may already provide these helper functions.
# Define them only when they do not already exist.
if(NOT COMMAND umicom_apply_warnings)
    function(umicom_apply_warnings target)
        if(NOT UMICOM_ENABLE_STRICT_WARNINGS)
            return()
        endif()

        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        else()
            target_compile_options(${target} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wshadow
            )
        endif()
    endfunction()
endif()

if(NOT COMMAND umicom_apply_sanitizers)
    function(umicom_apply_sanitizers target)
        if(UMICOM_ENABLE_SANITIZERS AND NOT MSVC)
            target_compile_options(${target} PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
            )
            target_link_options(${target} PRIVATE
                -fsanitize=address,undefined
            )
        endif()
    endfunction()
endif()

# -----------------------------------------------------------------------------
# Base library
# -----------------------------------------------------------------------------
add_library(umicom_base STATIC
    src/base/status.c
    src/base/result.c
    src/base/memory.c
    src/base/text.c
)
add_library(Umicom::base ALIAS umicom_base)
set_target_properties(umicom_base PROPERTIES EXPORT_NAME base)
target_include_directories(umicom_base PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_base)
umicom_apply_sanitizers(umicom_base)

# -----------------------------------------------------------------------------
# Diagnostics library
# -----------------------------------------------------------------------------
add_library(umicom_diagnostics STATIC
    src/diagnostics/diagnostic.c
    src/diagnostics/model.c
    src/diagnostics/filter.c
    src/diagnostics/query.c
    src/diagnostics/output.c
    src/diagnostics/output_buffer.c
    src/diagnostics/output_filter.c
    src/diagnostics/parser.c
    src/diagnostics/compiler_parser.c
    src/diagnostics/runtime_parser.c
    src/diagnostics/event.c
    src/diagnostics/pipeline.c
    src/diagnostics/log.c
    src/diagnostics/hub.c
    src/diagnostics/store.c
    src/diagnostics/metrics.c
    src/diagnostics/trace.c
    src/diagnostics/audit.c
    src/diagnostics/profiler.c
    src/diagnostics/readiness.c
    src/diagnostics/event_log.c
    src/diagnostics/privacy_filter.c
    src/diagnostics/crash_report.c
    src/diagnostics/exporter.c
    src/diagnostics/snapshot.c
)
add_library(Umicom::diagnostics ALIAS umicom_diagnostics)
set_target_properties(umicom_diagnostics PROPERTIES EXPORT_NAME diagnostics)
target_link_libraries(umicom_diagnostics PUBLIC Umicom::base)
target_include_directories(umicom_diagnostics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_diagnostics)
umicom_apply_sanitizers(umicom_diagnostics)

# -----------------------------------------------------------------------------
# Messaging library
# -----------------------------------------------------------------------------
add_library(umicom_messaging STATIC
    src/messaging/message.c
    src/messaging/payload.c
    src/messaging/schema.c
    src/messaging/channel.c
    src/messaging/subscription.c
    src/messaging/dispatcher.c
    src/messaging/reliability.c
    src/messaging/replay.c
    src/messaging/inbox.c
    src/messaging/outbox.c
    src/messaging/dead_letter.c
    src/messaging/idempotency.c
    src/messaging/delivery_lease.c
    src/messaging/transport.c
    src/messaging/router.c
    src/messaging/filter.c
    src/messaging/transformer.c
    src/messaging/aggregator.c
    src/messaging/resequencer.c
    src/messaging/flow.c
    src/messaging/workflow.c
    src/messaging/saga.c
    src/messaging/topic.c
    src/messaging/metrics.c
    src/messaging/event_bus.c
    src/messaging/command_bus.c
    src/messaging/query_bus.c
    src/messaging/journal.c
)
add_library(Umicom::messaging ALIAS umicom_messaging)
set_target_properties(umicom_messaging PROPERTIES EXPORT_NAME messaging)
target_link_libraries(umicom_messaging PUBLIC Umicom::base)
target_include_directories(umicom_messaging
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/messaging
)
umicom_apply_warnings(umicom_messaging)
umicom_apply_sanitizers(umicom_messaging)

# -----------------------------------------------------------------------------
# Data Server library
# -----------------------------------------------------------------------------
find_package(SQLite3 QUIET)

add_library(umicom_data STATIC
    src/data/data_server.c
    src/data/store.c
    src/data/repository.c
    src/data/transaction.c
    src/data/schema_version.c
    src/data/migration.c
    src/data/sqlite.c
    src/data/statement.c
    src/data/row.c
    src/data/blob_store.c
    src/data/cache.c
    src/data/metadata.c
    src/data/lineage.c
    src/data/backup.c
    src/data/integrity.c
    src/data/message_store.c
    src/data/journal_store.c
    src/data/explorer_types.c
    src/data/connection_profile.c
    src/data/schema_catalog.c
    src/data/query_policy.c
    src/data/query_history.c
    src/data/query_session.c
    src/data/explorer_transaction.c
    src/data/migration_catalog.c
    src/data/transfer_plan.c
    src/data/backup_catalog.c
    src/data/database_explorer.c
)
add_library(Umicom::data ALIAS umicom_data)
set_target_properties(umicom_data PROPERTIES EXPORT_NAME data)
target_link_libraries(umicom_data PUBLIC Umicom::base Umicom::messaging)
if(SQLite3_FOUND)
    if(TARGET SQLite3::SQLite3)
        target_link_libraries(umicom_data PUBLIC SQLite3::SQLite3)
    elseif(TARGET SQLite::SQLite3)
        target_link_libraries(umicom_data PUBLIC SQLite::SQLite3)
    else()
        target_include_directories(umicom_data PUBLIC ${SQLite3_INCLUDE_DIRS})
        target_link_libraries(umicom_data PUBLIC ${SQLite3_LIBRARIES})
    endif()
    target_compile_definitions(umicom_data PUBLIC UMICOM_HAS_SQLITE=1)
endif()
target_include_directories(umicom_data
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/data
)
umicom_apply_warnings(umicom_data)
umicom_apply_sanitizers(umicom_data)

# -----------------------------------------------------------------------------
# Platform services
# -----------------------------------------------------------------------------
add_library(umicom_platform STATIC
    src/platform/config.c
    src/platform/path.c
    src/platform/directory.c
    src/platform/cancellation.c
    src/platform/settings.c
    src/platform/filesystem.c
    src/platform/atomic_file.c
    src/platform/threading.c
    src/platform/task.c
    src/platform/task_queue.c
    src/platform/process.c
    src/platform/process_supervisor.c
    src/platform/watcher.c
    src/platform/watcher_polling.c
    src/platform/watcher_fake.c
    src/platform/workspace.c
    src/platform/document.c
    src/platform/document_store.c
    src/platform/workspace_graph.c
    src/platform/file_index.c
    src/platform/search.c
    src/platform/session_store.c
    src/platform/recovery.c
    src/platform/clock.c
    src/platform/recent_items.c
    src/platform/bookmarks.c
    src/platform/resource_location.c
    src/platform/workspace_history.c
    src/platform/file_operation_queue.c
    src/platform/resource_centre.c
)
add_library(Umicom::platform ALIAS umicom_platform)
set_target_properties(umicom_platform PROPERTIES EXPORT_NAME platform)
target_link_libraries(umicom_platform PUBLIC
    Umicom::base
    Umicom::finance
    Umicom::trading
    Umicom::diagnostics
    Threads::Threads
)
target_include_directories(umicom_platform
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)
umicom_apply_warnings(umicom_platform)
umicom_apply_sanitizers(umicom_platform)

# -----------------------------------------------------------------------------
# Capability-based security and secrets
# -----------------------------------------------------------------------------
add_library(umicom_security STATIC
    src/security/policy.c
    src/security/secrets.c
    src/security/identity.c
    src/security/permission.c
    src/security/role.c
    src/security/session.c
    src/security/credential.c
    src/security/token.c
    src/security/secret_provider.c
    src/security/redaction.c
    src/security/workspace_trust.c
    src/security/event.c
    src/security/authorisation.c
    src/security/context.c
    src/security/identity_directory.c
    src/security/role_policy.c
    src/security/role_assignment.c
    src/security/permission_catalogue.c
    src/security/workspace_policy.c
    src/security/secret_inventory.c
    src/security/redaction_policy.c
    src/security/approval.c
    src/security/package_trust.c
    src/security/security_audit.c
    src/security/security_centre.c
    src/security/security_catalogue.c
)
add_library(Umicom::security ALIAS umicom_security)
set_target_properties(umicom_security PROPERTIES EXPORT_NAME security)
target_link_libraries(umicom_security PUBLIC
    Umicom::base
    Umicom::platform
)
target_include_directories(umicom_security PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_security)
umicom_apply_sanitizers(umicom_security)

# -----------------------------------------------------------------------------
# Runtime, capability registry, application manifests, suites, modules,
# Master Controller and scheduler.
# -----------------------------------------------------------------------------
add_library(umicom_runtime STATIC
    src/runtime/capability_registry.c
    src/runtime/capability_catalogue.c
    src/runtime/service_registry.c
    src/runtime/command_registry.c
    src/runtime/inventory.c
    src/runtime/contract.c
    src/runtime/health.c
    src/runtime/application_manifest.c
    src/runtime/suite.c
    src/runtime/module_registry.c
    src/runtime/master_controller.c
    src/runtime/scheduler.c
)
add_library(Umicom::runtime ALIAS umicom_runtime)
set_target_properties(umicom_runtime PROPERTIES EXPORT_NAME runtime)
target_link_libraries(umicom_runtime PUBLIC
    Umicom::base
    Umicom::diagnostics
    Umicom::messaging
    Umicom::data
    Umicom::platform
    Umicom::security
)
target_include_directories(umicom_runtime PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_runtime)
umicom_apply_sanitizers(umicom_runtime)


# -----------------------------------------------------------------------------
# Suite and Inter-Application Runtime Foundation
# -----------------------------------------------------------------------------
add_library(umicom_integration STATIC
    src/integration/types.c
    src/integration/application.c
    src/integration/registry.c
    src/integration/resolver.c
    src/integration/presence.c
    src/integration/endpoint.c
    src/integration/route.c
    src/integration/router.c
    src/integration/envelope.c
    src/integration/dispatcher.c
    src/integration/session.c
    src/integration/suite.c
    src/integration/launch_plan.c
    src/integration/suite_runtime.c
    src/integration/topology.c
    src/integration/contract.c
    src/integration/health.c
    src/integration/bridge.c
    src/integration/catalogue.c
    src/integration/designer_types.c
    src/integration/connection_profile.c
    src/integration/api_contract.c
    src/integration/schema_catalogue.c
    src/integration/field_mapping.c
    src/integration/workflow.c
    src/integration/execution_policy.c
    src/integration/request_plan.c
    src/integration/simulator.c
    src/integration/run_history.c
    src/integration/designer_fabric.c
)
add_library(Umicom::integration ALIAS umicom_integration)
set_target_properties(umicom_integration PROPERTIES EXPORT_NAME integration)
target_link_libraries(umicom_integration PUBLIC
    Umicom::base
    Umicom::messaging
    Umicom::platform
    Umicom::security
    Umicom::runtime
)
target_include_directories(umicom_integration PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_integration)
umicom_apply_sanitizers(umicom_integration)

# -----------------------------------------------------------------------------
# Resilience, retry and supervision
# -----------------------------------------------------------------------------
add_library(umicom_resilience STATIC
    src/resilience/backoff.c
    src/resilience/retry.c
    src/resilience/circuit_breaker.c
    src/resilience/bulkhead.c
    src/resilience/rate_limiter.c
    src/resilience/restart_policy.c
    src/resilience/deadline.c
    src/resilience/fault.c
    src/resilience/health_gate.c
    src/resilience/supervisor.c
)
add_library(Umicom::resilience ALIAS umicom_resilience)
set_target_properties(umicom_resilience PROPERTIES EXPORT_NAME resilience)
target_link_libraries(umicom_resilience PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::runtime
)
target_include_directories(umicom_resilience PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_resilience)
umicom_apply_sanitizers(umicom_resilience)

# -----------------------------------------------------------------------------
# Operations, observability, performance and resilience evidence
# -----------------------------------------------------------------------------
add_library(umicom_observability STATIC
    src/observability/resource_history.c
    src/observability/benchmark_history.c
    src/observability/crash_registry.c
    src/observability/correlation.c
    src/observability/health_summary.c
    src/observability/resilience_telemetry.c
    src/observability/operations_centre.c
    src/observability/operations_catalogue.c
)
add_library(Umicom::observability ALIAS umicom_observability)
set_target_properties(umicom_observability PROPERTIES EXPORT_NAME observability)
target_link_libraries(umicom_observability PUBLIC
    Umicom::base
    Umicom::diagnostics
    Umicom::runtime
    Umicom::resilience
)
target_include_directories(umicom_observability PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_observability)
umicom_apply_sanitizers(umicom_observability)

# -----------------------------------------------------------------------------
# Toolkit-neutral UI contracts and reference frontend adapters
# -----------------------------------------------------------------------------
add_library(umicom_ui STATIC
    src/ui/types.c
    src/ui/value.c
    src/ui/property.c
    src/ui/view_model.c
    src/ui/action.c
    src/ui/menu.c
    src/ui/toolbar.c
    src/ui/status.c
    src/ui/notification.c
    src/ui/dialog.c
    src/ui/theme.c
    src/ui/selection.c
    src/ui/clipboard.c
    src/ui/accessibility.c
    src/ui/pane.c
    src/ui/document_view.c
    src/ui/document_layout.c
    src/ui/editor.c
    src/ui/editor_theme.c
    src/ui/perspective.c
    src/ui/layout.c
    src/ui/contribution.c
    src/ui/binding.c
    src/ui/event.c
    src/ui/adapter.c
    src/ui/view_factory.c
    src/ui/activity.c
    src/ui/view_container.c
    src/ui/context.c
    src/ui/keybinding.c
    src/ui/quick_access.c
    src/ui/breadcrumb.c
    src/ui/explorer.c
    src/ui/workbench_state.c
    src/ui/workspace_profile.c
    src/ui/workspace_profile_persistence.c
    src/ui/appearance.c
    src/ui/appearance_persistence.c
    src/ui/view_presentation.c
    src/ui/list_model.c
    src/ui/tree_model.c
    src/ui/selection_model.c
    src/ui/sort_filter_model.c
    src/ui/dock_model.c
    src/ui/tab_model.c
    src/ui/panel_model.c
    src/ui/context_menu.c
    src/ui/welcome_view.c
    src/ui/property_inspector.c
    src/ui/drag_drop.c
    src/ui/extension_point.c
    src/ui/command_surface.c
    src/ui/command_view.c
    src/ui/undo_stack.c
    src/ui/workbench_platform.c
    src/ui/problem.c
    src/ui/output_channel.c
    src/ui/progress.c
    src/ui/task_monitor.c
    src/ui/notification_item.c
    src/ui/status_item.c
    src/ui/command_history.c
    src/ui/navigation_stack.c
    src/ui/view_state.c
    src/ui/workbench_services.c
    src/ui/workbench.c
    src/ui/application_shell.c
    src/ui/workspace_layout.c
    src/ui/window_catalogue.c
    src/ui/window_group.c
    src/ui/layout_library.c
    src/ui/theme_profile.c
    src/ui/layout_persistence.c
    src/ui/workspace_customisation.c
)
add_library(Umicom::ui ALIAS umicom_ui)
set_target_properties(umicom_ui PROPERTIES EXPORT_NAME ui)
target_link_libraries(umicom_ui PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::runtime
)
target_include_directories(umicom_ui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)
umicom_apply_warnings(umicom_ui)
umicom_apply_sanitizers(umicom_ui)


# -----------------------------------------------------------------------------
# Toolkit-neutral reusable UI component catalogue
# -----------------------------------------------------------------------------
add_library(umicom_ui_components STATIC
    src/ui/components/component.c
    src/ui/components/catalogue.c
    src/ui/components/registry.c
    src/ui/components/tree.c
    src/ui/components/layout.c
    src/ui/components/style.c
    src/ui/components/state.c
    src/ui/components/command.c
    src/ui/components/property.c
    src/ui/components/validation.c
    src/ui/components/factory.c
    src/ui/components/model.c
    src/ui/components/accessibility.c
    src/ui/components/collection.c
    src/ui/components/form.c
    src/ui/components/selection.c
    src/ui/components/surface.c
    src/ui/components/host.c
    src/ui/components/form_validation.c
    src/ui/components/component_set.c
    src/ui/components/gtk4_widget_catalogue.c
)
add_library(Umicom::ui_components ALIAS umicom_ui_components)
set_target_properties(umicom_ui_components PROPERTIES EXPORT_NAME ui_components)
target_link_libraries(umicom_ui_components PUBLIC Umicom::ui)
target_include_directories(umicom_ui_components PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_ui_components)
umicom_apply_sanitizers(umicom_ui_components)

add_library(umicom_ui_headless STATIC
    adapters/headless/headless_adapter.c
    adapters/headless/shell_headless.c
    adapters/headless/workbench_headless.c
    adapters/headless/panes_headless.c
    adapters/headless/view_model_headless.c
    adapters/headless/documents_headless.c
    adapters/headless/status_headless.c
    adapters/headless/notifications_headless.c
    adapters/headless/dialogs_headless.c
    adapters/headless/selection_headless.c
)
add_library(Umicom::ui_headless ALIAS umicom_ui_headless)
set_target_properties(umicom_ui_headless PROPERTIES EXPORT_NAME ui_headless)
target_link_libraries(umicom_ui_headless PUBLIC Umicom::ui Umicom::ui_components)
target_include_directories(umicom_ui_headless
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/adapters/headless
)
umicom_apply_warnings(umicom_ui_headless)
umicom_apply_sanitizers(umicom_ui_headless)

if(UMICOM_BUILD_GTK4_ADAPTER)
    find_package(PkgConfig REQUIRED)
    # GtkFileDialog replaced deprecated GtkFileChooser APIs in GTK 4.10.  Make
    # the API floor explicit so strict builds never fall back to deprecated UI.
    pkg_check_modules(UMICOM_FRAMEWORK_GTK4 REQUIRED IMPORTED_TARGET gtk4>=4.10)
    pkg_check_modules(UMICOM_FRAMEWORK_SOURCEVIEW5 QUIET IMPORTED_TARGET gtksourceview-5)
    add_library(umicom_ui_gtk4 STATIC
        adapters/gtk4/gtk4_adapter.c
        adapters/gtk4/shell_gtk4.c
        adapters/gtk4/workbench_gtk4.c
        adapters/gtk4/workspace_profile_gtk4.c
        adapters/gtk4/desktop_shell_gtk4.c
        adapters/gtk4/desktop_designer_gtk4.c
        adapters/gtk4/appearance_gtk4.c
        adapters/gtk4/activity_bar_gtk4.c
        adapters/gtk4/view_container_gtk4.c
        adapters/gtk4/breadcrumb_gtk4.c
        adapters/gtk4/quick_access_gtk4.c
        adapters/gtk4/keybinding_gtk4.c
        adapters/gtk4/view_model_gtk4.c
        adapters/gtk4/problems_gtk4.c
        adapters/gtk4/output_gtk4.c
        adapters/gtk4/terminal_gtk4.c
        adapters/gtk4/process_gtk4.c
        adapters/gtk4/task_gtk4.c
        adapters/gtk4/source_control_gtk4.c
        adapters/gtk4/pane_gtk4.c
        adapters/gtk4/editor_gtk4.c
        adapters/gtk4/editor_theme_gtk4.c
        adapters/gtk4/action_dispatch_gtk4.c
        adapters/gtk4/menu_gtk4.c
        adapters/gtk4/toolbar_gtk4.c
        adapters/gtk4/status_gtk4.c
        adapters/gtk4/notification_gtk4.c
        adapters/gtk4/theme_gtk4.c
        adapters/gtk4/accessibility_gtk4.c
        adapters/gtk4/selection_gtk4.c
        adapters/gtk4/dialog_gtk4.c
        adapters/gtk4/clipboard_gtk4.c
        adapters/gtk4/declarative_renderer_gtk4.c
        adapters/gtk4/components/component_factory_gtk4.c
        adapters/gtk4/components/controls_gtk4.c
        adapters/gtk4/components/containers_gtk4.c
        adapters/gtk4/components/navigation_gtk4.c
        adapters/gtk4/components/window_gtk4.c
        adapters/gtk4/components/views_gtk4.c
        adapters/gtk4/components/media_gtk4.c
        adapters/gtk4/components/feedback_gtk4.c
        adapters/gtk4/components/catalogue_gtk4.c
        adapters/gtk4/components/native_factory_gtk4.c
    )
    add_library(Umicom::ui_gtk4 ALIAS umicom_ui_gtk4)
    set_target_properties(umicom_ui_gtk4 PROPERTIES EXPORT_NAME ui_gtk4)
    target_link_libraries(umicom_ui_gtk4 PUBLIC
        Umicom::ui
        Umicom::ui_components
        PkgConfig::UMICOM_FRAMEWORK_GTK4
    )
    target_link_libraries(umicom_ui_gtk4 PRIVATE Umicom::editor)
    if(TARGET PkgConfig::UMICOM_FRAMEWORK_SOURCEVIEW5)
        target_link_libraries(umicom_ui_gtk4 PRIVATE
            PkgConfig::UMICOM_FRAMEWORK_SOURCEVIEW5)
        target_compile_definitions(umicom_ui_gtk4 PRIVATE
            UMICOM_GTK4_HAS_SOURCEVIEW5=1)
    endif()
    target_include_directories(umicom_ui_gtk4
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/adapters/gtk4
    )
    umicom_apply_warnings(umicom_ui_gtk4)
    if(UMICOM_ENABLE_STRICT_WARNINGS AND
       CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        set_source_files_properties(
            adapters/gtk4/keybinding_gtk4.c
            adapters/gtk4/editor_gtk4.c
            adapters/gtk4/editor_theme_gtk4.c
            adapters/gtk4/action_dispatch_gtk4.c
            adapters/gtk4/source_control_gtk4.c
            PROPERTIES COMPILE_OPTIONS
                "-Werror=pedantic;-Werror=conversion;-Werror=deprecated-declarations")
    endif()
    umicom_apply_sanitizers(umicom_ui_gtk4)
endif()

# -----------------------------------------------------------------------------
# Declarative application language and compiler
# -----------------------------------------------------------------------------
add_library(umicom_declarative STATIC
    src/declarative/types.c
    src/declarative/value.c
    src/declarative/attribute.c
    src/declarative/node.c
    src/declarative/document.c
    src/declarative/diagnostic.c
    src/declarative/lexer.c
    src/declarative/parser.c
    src/declarative/serializer.c
    src/declarative/formatter.c
    src/declarative/property.c
    src/declarative/component.c
    src/declarative/component_registry.c
    src/declarative/catalogue.c
    src/declarative/binding.c
    src/declarative/command.c
    src/declarative/event.c
    src/declarative/frontend.c
    src/declarative/starter.c
    src/declarative/schema.c
    src/declarative/validator.c
    src/declarative/plan.c
    src/declarative/compiler.c
    src/declarative/migration.c
    src/declarative/template_store.c
    src/declarative/renderer.c
    src/declarative/generator.c
    adapters/headless/declarative_renderer_headless.c
)
add_library(Umicom::declarative ALIAS umicom_declarative)
set_target_properties(umicom_declarative PROPERTIES EXPORT_NAME declarative)
target_link_libraries(umicom_declarative PUBLIC Umicom::base)
target_include_directories(umicom_declarative PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_declarative)
umicom_apply_sanitizers(umicom_declarative)

# -----------------------------------------------------------------------------
# Reusable visual designer engine
# -----------------------------------------------------------------------------
add_library(umicom_designer STATIC
    src/designer/types.c
    src/designer/document.c
    src/designer/operation.c
    src/designer/history.c
    src/designer/selection.c
    src/designer/palette.c
    src/designer/inspector.c
    src/designer/surface.c
    src/designer/preview.c
    src/designer/project.c
    src/designer/catalogue.c
    src/designer/command.c
    src/designer/signal_binding.c
    src/designer/action_binding.c
    src/designer/property_schema.c
    src/designer/alignment.c
    src/designer/clipboard.c
    src/designer/template_palette.c
    src/designer/authoring_session.c
    src/designer/builder_types.c
    src/designer/component_tree.c
    src/designer/property_editor.c
    src/designer/binding_editor.c
    src/designer/interaction_editor.c
    src/designer/drag_drop.c
    src/designer/transaction.c
    src/designer/semantic_clipboard.c
    src/designer/template.c
    src/designer/layout.c
    src/designer/live_preview.c
    src/designer/source_generation.c
    src/designer/builder_session.c
    adapters/headless/designer_preview_headless.c
)
add_library(Umicom::designer ALIAS umicom_designer)
set_target_properties(umicom_designer PROPERTIES EXPORT_NAME designer)
target_link_libraries(umicom_designer PUBLIC
    Umicom::declarative
    Umicom::runtime
)
target_include_directories(umicom_designer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_designer)
umicom_apply_sanitizers(umicom_designer)

# -----------------------------------------------------------------------------
# Toolchain discovery, child environments, dependency checks and build service.
# -----------------------------------------------------------------------------
add_library(umicom_toolchain STATIC
    src/toolchain/tool.c
    src/toolchain/profile.c
    src/toolchain/discovery.c
    src/toolchain/environment.c
    src/toolchain/dependency.c
    src/toolchain/build.c
    src/toolchain/capability.c
    src/toolchain/catalogue.c
    src/toolchain/sdk.c
    src/toolchain/compilation_database.c
)
add_library(Umicom::toolchain ALIAS umicom_toolchain)
set_target_properties(umicom_toolchain PROPERTIES EXPORT_NAME toolchain)
target_link_libraries(umicom_toolchain PUBLIC
    Umicom::base
    Umicom::diagnostics
    Umicom::platform
)
target_include_directories(umicom_toolchain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_toolchain)
umicom_apply_sanitizers(umicom_toolchain)

# -----------------------------------------------------------------------------
# Batch 58 compiler, polyglot and Umicc integration contracts.
# -----------------------------------------------------------------------------
add_library(umicom_compiler STATIC
    src/compiler/language.c
    src/compiler/target.c
    src/compiler/request.c
    src/compiler/plan.c
    src/compiler/provider_defaults.c
    src/compiler/provider.c
    src/compiler/registry.c
    src/compiler/gcc_provider.c
    src/compiler/clang_provider.c
    src/compiler/msvc_provider.c
    src/compiler/rust_provider.c
    src/compiler/zig_provider.c
    src/compiler/assembly_provider.c
    src/compiler/uai_provider.c
    src/compiler/umicc_provider.c
    src/compiler/linker_profile.c
    src/compiler/diagnostic.c
    src/compiler/artifact.c
    src/compiler/abi.c
    src/compiler/interop_graph.c
    src/compiler/build_graph.c
    src/compiler/response_file.c
    src/compiler/compilation_database.c
    src/compiler/policy.c
    src/compiler/profile.c
    src/compiler/capability.c
    src/compiler/manifest.c
    src/compiler/lockfile.c
    src/compiler/sysroot.c
    src/compiler/runtime_library.c
    src/compiler/cache.c
    src/compiler/toolchain_bridge.c
    src/compiler/centre.c
)
add_library(Umicom::compiler ALIAS umicom_compiler)
set_target_properties(umicom_compiler PROPERTIES EXPORT_NAME compiler)
target_link_libraries(umicom_compiler PUBLIC
    Umicom::base
    Umicom::toolchain
)
target_include_directories(umicom_compiler
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler
)
umicom_apply_warnings(umicom_compiler)
umicom_apply_sanitizers(umicom_compiler)


# -----------------------------------------------------------------------------
# Build orchestration
# -----------------------------------------------------------------------------
add_library(umicom_build STATIC
    src/build/types.c
    src/build/profile.c
    src/build/preset.c
    src/build/diagnostic.c
    src/build/parser.c
    src/build/result.c
    src/build/history.c
    src/build/provider.c
    src/build/cmake_provider.c
    src/build/ninja_provider.c
    src/build/ctest_provider.c
    src/build/runner.c
    src/build/graph.c
    src/build/policy.c
    src/build/plan.c
    src/build/engine.c
    src/build/artifact.c
    src/build/workspace.c
)
add_library(Umicom::build ALIAS umicom_build)
set_target_properties(umicom_build PROPERTIES EXPORT_NAME build)
target_link_libraries(umicom_build PUBLIC
    Umicom::base
    Umicom::diagnostics
    Umicom::platform
)
target_include_directories(umicom_build PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_build)
umicom_apply_sanitizers(umicom_build)

# Framework-owned professional build-and-task presentation. Applications bind
# their existing build objects into one workspace and reuse these factories in
# GTK4, Qt, Wt, headless automation and future frontend adapters.
add_library(umicom_build_ui STATIC
    src/build_ui/build_views.c
)
add_library(Umicom::build_ui ALIAS umicom_build_ui)
set_target_properties(umicom_build_ui PROPERTIES EXPORT_NAME build_ui)
target_link_libraries(umicom_build_ui PUBLIC
    Umicom::build
    Umicom::ui
)
target_include_directories(umicom_build_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_build_ui)
umicom_apply_sanitizers(umicom_build_ui)

# -----------------------------------------------------------------------------
# Framework-owned diagnostics presentation and build projection
# -----------------------------------------------------------------------------
add_library(umicom_diagnostic_ui STATIC
    src/diagnostic_ui/projection.c
    src/diagnostic_ui/build_projection.c
    src/diagnostic_ui/problems_view.c
    src/diagnostic_ui/output_view.c
    src/diagnostic_ui/navigation.c
)
add_library(Umicom::diagnostic_ui ALIAS umicom_diagnostic_ui)
set_target_properties(umicom_diagnostic_ui PROPERTIES EXPORT_NAME diagnostic_ui)
target_link_libraries(umicom_diagnostic_ui PUBLIC
    Umicom::diagnostics
    Umicom::ui
    Umicom::build
)
target_include_directories(umicom_diagnostic_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_diagnostic_ui)
umicom_apply_sanitizers(umicom_diagnostic_ui)

if(TARGET umicom_ui_gtk4)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::diagnostic_ui)
endif()

# -----------------------------------------------------------------------------
# Test discovery and execution
# -----------------------------------------------------------------------------
add_library(umicom_testing STATIC
    src/testing/types.c
    src/testing/case.c
    src/testing/result.c
    src/testing/suite.c
    src/testing/registry.c
    src/testing/discovery.c
    src/testing/runner.c
    src/testing/report.c
    src/testing/ctest_adapter.c
)
add_library(Umicom::testing ALIAS umicom_testing)
set_target_properties(umicom_testing PROPERTIES EXPORT_NAME testing)
target_link_libraries(umicom_testing PUBLIC
    Umicom::base
    Umicom::platform
)
target_include_directories(umicom_testing PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_testing)
umicom_apply_sanitizers(umicom_testing)

# -----------------------------------------------------------------------------
# Terminal sessions and prepared shells
# -----------------------------------------------------------------------------
set(UMICOM_TERMINAL_ASSEMBLY_SOURCE "")
set(UMICOM_TERMINAL_ASSEMBLY_DEFINITION "")
if(UMICOM_TERMINAL_ENABLE_ASSEMBLY AND
   CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
        enable_language(ASM)
        set(UMICOM_TERMINAL_ASSEMBLY_SOURCE
            src/terminal/asm/scan_x86_64.S)
        set(UMICOM_TERMINAL_ASSEMBLY_DEFINITION
            UMICOM_TERMINAL_SCAN_X86_64_ASM=1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
        enable_language(ASM)
        set(UMICOM_TERMINAL_ASSEMBLY_SOURCE
            src/terminal/asm/scan_aarch64.S)
        set(UMICOM_TERMINAL_ASSEMBLY_DEFINITION
            UMICOM_TERMINAL_SCAN_AARCH64_ASM=1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv64|RISCV64)$")
        enable_language(ASM)
        set(UMICOM_TERMINAL_ASSEMBLY_SOURCE
            src/terminal/asm/scan_riscv64.S)
        set(UMICOM_TERMINAL_ASSEMBLY_DEFINITION
            UMICOM_TERMINAL_SCAN_RISCV64_ASM=1)
    endif()
endif()

add_library(umicom_terminal STATIC
    src/terminal/types.c
    src/terminal/command.c
    src/terminal/transcript.c
    src/terminal/environment.c
    src/terminal/session.c
    src/terminal/manager.c
    src/terminal/shell.c
    src/terminal/profile.c
    src/terminal/profile_registry.c
    src/terminal/history.c
    src/terminal/scan.c
    src/terminal/scan_portable.c
    src/terminal/search.c
    src/terminal/link.c
    src/terminal/ansi.c
    src/terminal/tab_model.c
    src/terminal/split_model.c
    src/terminal/event.c
    src/terminal/metrics.c
    ${UMICOM_TERMINAL_ASSEMBLY_SOURCE}
)
add_library(Umicom::terminal ALIAS umicom_terminal)
set_target_properties(umicom_terminal PROPERTIES EXPORT_NAME terminal)
target_link_libraries(umicom_terminal PUBLIC
    Umicom::base
    Umicom::platform
)
target_include_directories(umicom_terminal PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(UMICOM_TERMINAL_ASSEMBLY_DEFINITION)
    target_compile_definitions(umicom_terminal PRIVATE
        ${UMICOM_TERMINAL_ASSEMBLY_DEFINITION})
endif()
umicom_apply_warnings(umicom_terminal)
umicom_apply_sanitizers(umicom_terminal)

# Framework-owned terminal, process and task workbench presentation.
add_library(umicom_terminal_ui STATIC
    src/terminal_ui/view_internal.c
    src/terminal_ui/controller.c
    src/terminal_ui/terminal_view.c
    src/terminal_ui/process_view.c
    src/terminal_ui/task_view.c
    src/terminal_ui/history_view.c
    src/terminal_ui/navigation.c
)
add_library(Umicom::terminal_ui ALIAS umicom_terminal_ui)
set_target_properties(umicom_terminal_ui PROPERTIES EXPORT_NAME terminal_ui)
target_link_libraries(umicom_terminal_ui PUBLIC
    Umicom::build
    Umicom::terminal
    Umicom::diagnostics
    Umicom::ui
)
target_include_directories(umicom_terminal_ui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_ui
)
umicom_apply_warnings(umicom_terminal_ui)
umicom_apply_sanitizers(umicom_terminal_ui)

if(TARGET umicom_ui_gtk4)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::terminal_ui)
endif()

# -----------------------------------------------------------------------------
# JSON-RPC, LSP and DAP protocol foundation
# -----------------------------------------------------------------------------
add_library(umicom_protocol STATIC
    src/protocol/types.c
    src/protocol/framing.c
    src/protocol/json.c
    src/protocol/json_rpc.c
    src/protocol/transport.c
    src/protocol/client.c
    src/protocol/lsp.c
    src/protocol/lsp_document.c
    src/protocol/dap.c
    src/protocol/dap_breakpoint.c
    src/protocol/lsp_request_internal.c
    src/protocol/lsp_did_close.c
    src/protocol/lsp_did_save.c
    src/protocol/lsp_hover.c
    src/protocol/lsp_signature_help.c
    src/protocol/lsp_definition.c
    src/protocol/lsp_implementation.c
    src/protocol/lsp_type_definition.c
    src/protocol/lsp_references.c
    src/protocol/lsp_document_symbols.c
    src/protocol/lsp_workspace_symbols.c
    src/protocol/lsp_rename.c
    src/protocol/lsp_prepare_rename.c
    src/protocol/lsp_formatting.c
    src/protocol/lsp_range_formatting.c
    src/protocol/lsp_code_action.c
    src/protocol/lsp_semantic_tokens.c
    src/protocol/lsp_inlay_hints.c
    src/protocol/lsp_folding_ranges.c
    src/protocol/lsp_document_highlights.c
    src/protocol/lsp_execute_command.c
    src/protocol/dap_request_internal.c
    src/protocol/dap_attach.c
    src/protocol/dap_configuration_done.c
    src/protocol/dap_set_breakpoints.c
    src/protocol/dap_pause.c
    src/protocol/dap_next.c
    src/protocol/dap_step_in.c
    src/protocol/dap_step_out.c
    src/protocol/dap_threads.c
    src/protocol/dap_stack_trace.c
    src/protocol/dap_scopes.c
    src/protocol/dap_variables.c
    src/protocol/dap_evaluate.c
    src/protocol/dap_set_variable.c
    src/protocol/dap_restart.c
    src/protocol/dap_terminate.c
    src/protocol/dap_source.c
    src/protocol/dap_modules.c
    src/protocol/dap_exception_info.c
    src/protocol/dap_loaded_sources.c
    src/protocol/dap_read_memory.c
    src/protocol/dap_disassemble.c
)
add_library(Umicom::protocol ALIAS umicom_protocol)
set_target_properties(umicom_protocol PROPERTIES EXPORT_NAME protocol)
target_link_libraries(umicom_protocol PUBLIC
    Umicom::base
    Umicom::platform
)
target_include_directories(umicom_protocol PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_protocol)
umicom_apply_sanitizers(umicom_protocol)

# -----------------------------------------------------------------------------
# Reusable text-editor models and editor session
# -----------------------------------------------------------------------------
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomEditorFeatures.cmake")

set(UMICOM_EDITOR_ASSEMBLY_SOURCE "")
set(UMICOM_EDITOR_ASSEMBLY_DEFINITION "")
if(UMICOM_EDITOR_ENABLE_ASSEMBLY AND
   CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
        enable_language(ASM)
        set(UMICOM_EDITOR_ASSEMBLY_SOURCE
            src/editor/asm/text_scan_x86_64.S)
        set(UMICOM_EDITOR_ASSEMBLY_DEFINITION
            UMICOM_EDITOR_SCAN_X86_64_ASM=1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
        enable_language(ASM)
        set(UMICOM_EDITOR_ASSEMBLY_SOURCE
            src/editor/asm/text_scan_aarch64.S)
        set(UMICOM_EDITOR_ASSEMBLY_DEFINITION
            UMICOM_EDITOR_SCAN_AARCH64_ASM=1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv64|RISCV64)$")
        enable_language(ASM)
        set(UMICOM_EDITOR_ASSEMBLY_SOURCE
            src/editor/asm/text_scan_riscv64.S)
        set(UMICOM_EDITOR_ASSEMBLY_DEFINITION
            UMICOM_EDITOR_SCAN_RISCV64_ASM=1)
    endif()
endif()

add_library(umicom_editor STATIC
    src/editor/feature.c
    src/editor/feature_builtin.c
    src/editor/text_buffer.c
    src/editor/line_index.c
    src/editor/search_engine.c
    src/editor/edit_history.c
    src/editor/edit_command.c
    src/editor/edit_engine.c
    src/editor/source_location.c
    src/editor/navigation_history.c
    src/editor/navigation_result.c
    src/editor/source_bookmark.c
    src/editor/symbol_index.c
    src/editor/symbol_relationship.c
    src/editor/document_outline.c
    src/editor/breadcrumb_model.c
    src/editor/source_navigation.c
    src/editor/navigation_command.c
    src/editor/peek_definition.c
    src/editor/reference_results.c
    src/editor/symbol_hierarchy.c
    src/editor/call_hierarchy.c
    src/editor/type_hierarchy.c
    src/editor/navigation_insights.c
    src/editor/workspace_edit.c
    src/editor/rename_symbol.c
    src/editor/code_lens_model.c
    src/editor/inlay_hint_model.c
    src/editor/semantic_highlighting.c
    src/editor/editor_intelligence.c
    src/editor/intelligence_command.c
    src/editor/hover_model.c
    src/editor/signature_help_model.c
    src/editor/parameter_information.c
    src/editor/quick_documentation.c
    src/editor/editor_assistance.c
    src/editor/assistance_command.c
    src/editor/formatting_profile.c
    src/editor/formatting_session.c
    src/editor/import_organisation.c
    src/editor/snippet_session.c
    src/editor/linked_editing_model.c
    src/editor/editing_productivity.c
    src/editor/productivity_command.c
    src/editor/code_action_provider.c
    src/editor/code_action_query.c
    src/editor/refactoring_plan.c
    src/editor/refactoring_preview.c
    src/editor/edit_transaction.c
    src/editor/code_action_orchestration.c
    src/editor/refactoring_command.c
    src/editor/text_scan.c
    src/editor/text_scan_portable.c
    src/editor/document.c
    src/editor/cursor.c
    src/editor/selection_range.c
    src/editor/marker.c
    src/editor/fold_region.c
    src/editor/symbol.c
    src/editor/diagnostic.c
    src/editor/completion.c
    src/editor/code_action.c
    src/editor/diff_hunk.c
    src/editor/configuration.c
    src/editor/presentation.c
    src/editor/session.c
    ${UMICOM_EDITOR_FEATURE_SOURCES}
    ${UMICOM_EDITOR_ASSEMBLY_SOURCE}
)
add_library(Umicom::editor ALIAS umicom_editor)
set_target_properties(umicom_editor PROPERTIES EXPORT_NAME editor)
target_link_libraries(umicom_editor PUBLIC Umicom::base)
target_include_directories(umicom_editor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(UMICOM_EDITOR_ASSEMBLY_DEFINITION)
    target_compile_definitions(umicom_editor PRIVATE
        ${UMICOM_EDITOR_ASSEMBLY_DEFINITION})
endif()
umicom_apply_warnings(umicom_editor)
umicom_apply_sanitizers(umicom_editor)

# -----------------------------------------------------------------------------
# Framework-owned document I/O, working-copy and command platform
# -----------------------------------------------------------------------------
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomDocumentCapabilities.cmake")

set(UMICOM_DOCUMENT_ASSEMBLY_SOURCE "")
set(UMICOM_DOCUMENT_ASSEMBLY_DEFINITION "")
if(UMICOM_EDITOR_ENABLE_ASSEMBLY AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
        enable_language(ASM)
        set(UMICOM_DOCUMENT_ASSEMBLY_SOURCE
            src/document/asm/fingerprint_x86_64.S)
        set(UMICOM_DOCUMENT_ASSEMBLY_DEFINITION
            UMICOM_DOCUMENT_HASH_X86_64_ASM=1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
        enable_language(ASM)
        set(UMICOM_DOCUMENT_ASSEMBLY_SOURCE
            src/document/asm/fingerprint_aarch64.S)
        set(UMICOM_DOCUMENT_ASSEMBLY_DEFINITION
            UMICOM_DOCUMENT_HASH_AARCH64_ASM=1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv64|RISCV64)$")
        enable_language(ASM)
        set(UMICOM_DOCUMENT_ASSEMBLY_SOURCE
            src/document/asm/fingerprint_riscv64.S)
        set(UMICOM_DOCUMENT_ASSEMBLY_DEFINITION
            UMICOM_DOCUMENT_HASH_RISCV64_ASM=1)
    endif()
endif()

add_library(umicom_document STATIC
    src/document/types.c
    src/document/capability.c
    src/document/capability_builtin.c
    src/document/text_encoding.c
    src/document/line_endings.c
    src/document/uri.c
    src/document/fingerprint.c
    src/document/fingerprint_portable.c
    src/document/file_identity.c
    src/document/provider.c
    src/document/local_provider.c
    src/document/loader.c
    src/document/saver.c
    src/document/language_detection.c
    src/document/file_filter.c
    src/document/coordinator.c
    src/document/commands.c
    ${UMICOM_DOCUMENT_CAPABILITY_SOURCES}
    ${UMICOM_DOCUMENT_ASSEMBLY_SOURCE}
)
add_library(Umicom::document ALIAS umicom_document)
set_target_properties(umicom_document PROPERTIES EXPORT_NAME document)
target_link_libraries(umicom_document PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::runtime
    Umicom::ui
    Umicom::editor
)
target_include_directories(umicom_document PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(UMICOM_DOCUMENT_ASSEMBLY_DEFINITION)
    target_compile_definitions(umicom_document PRIVATE
        ${UMICOM_DOCUMENT_ASSEMBLY_DEFINITION})
endif()
umicom_apply_warnings(umicom_document)
umicom_apply_sanitizers(umicom_document)

# -----------------------------------------------------------------------------
# Provider-neutral version control and Git CLI adapter
# -----------------------------------------------------------------------------
add_library(umicom_vcs STATIC
    src/vcs/types.c
    src/vcs/change.c
    src/vcs/status.c
    src/vcs/branch.c
    src/vcs/branch_list.c
    src/vcs/commit.c
    src/vcs/history.c
    src/vcs/diff.c
    src/vcs/diff_document.c
    src/vcs/remote.c
    src/vcs/tag.c
    src/vcs/conflict.c
    src/vcs/operation_log.c
    src/vcs/provider.c
    src/vcs/repository.c
    src/vcs/git_cli.c
    src/vcs/workspace.c
    src/vcs/workspace_coordinator.c
)
add_library(Umicom::vcs ALIAS umicom_vcs)
set_target_properties(umicom_vcs PROPERTIES EXPORT_NAME vcs)
target_link_libraries(umicom_vcs PUBLIC
    Umicom::base
    Umicom::platform
)
target_include_directories(umicom_vcs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_vcs)
umicom_apply_sanitizers(umicom_vcs)

# Framework-owned Source Control Centre presentation shared by Studio and
# every future Framework application.  Toolkit adapters consume these models.
add_library(umicom_vcs_ui STATIC
    src/vcs_ui/view_internal.c
    src/vcs_ui/source_control_view.c
    src/vcs_ui/history_view.c
    src/vcs_ui/branches_view.c
    src/vcs_ui/remotes_view.c
    src/vcs_ui/diff_view.c
    src/vcs_ui/workspace_views.c
)
add_library(Umicom::vcs_ui ALIAS umicom_vcs_ui)
set_target_properties(umicom_vcs_ui PROPERTIES EXPORT_NAME vcs_ui)
target_link_libraries(umicom_vcs_ui PUBLIC Umicom::vcs Umicom::ui)
target_include_directories(umicom_vcs_ui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vcs_ui
)
umicom_apply_warnings(umicom_vcs_ui)
umicom_apply_sanitizers(umicom_vcs_ui)
if(TARGET umicom_ui_gtk4)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::vcs_ui)
endif()

# -----------------------------------------------------------------------------
# Native local Git and optional GitHub repository operations.
# -----------------------------------------------------------------------------
add_library(umicom_repository STATIC
    src/repository/repository.c
)
add_library(Umicom::repository ALIAS umicom_repository)
set_target_properties(umicom_repository PROPERTIES EXPORT_NAME repository)
target_link_libraries(umicom_repository PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::toolchain
)
target_include_directories(umicom_repository PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_repository)
umicom_apply_sanitizers(umicom_repository)

# -----------------------------------------------------------------------------
# Plug-in boundary
# -----------------------------------------------------------------------------
add_library(umicom_plugin STATIC
    src/plugin/plugin.c
    src/plugin/state.c
    src/plugin/manifest.c
    src/plugin/permission.c
    src/plugin/contribution.c
    src/plugin/registry.c
    src/plugin/checksum.c
    src/plugin/signature.c
    src/plugin/compatibility.c
    src/plugin/discovery.c
    src/plugin/loader.c
    src/plugin/lifecycle.c
    src/plugin/host.c
    src/plugin/dependency.c
    src/plugin/extension_point.c
    src/plugin/policy.c
    src/plugin/package.c
    src/plugin/event.c
    src/plugin/catalogue.c
    src/plugin/sdk.c
    src/plugin/manager.c
)
add_library(Umicom::plugin ALIAS umicom_plugin)
set_target_properties(umicom_plugin PROPERTIES EXPORT_NAME plugin)
target_link_libraries(umicom_plugin PUBLIC
    Umicom::base
    Umicom::runtime
    Umicom::security
    Umicom::resilience
)
target_include_directories(umicom_plugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(UNIX AND NOT APPLE)
    target_link_libraries(umicom_plugin PRIVATE dl)
endif()
umicom_apply_warnings(umicom_plugin)
if(UMICOM_ENABLE_STRICT_WARNINGS AND
   CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(src/plugin/plugin.c PROPERTIES
        COMPILE_OPTIONS "-Werror=pedantic")
endif()
umicom_apply_sanitizers(umicom_plugin)

# Toolkit-neutral Extension Centre projections.  Product frontends consume
# these view models without acquiring ownership of plug-in policy or state.
add_library(umicom_plugin_ui STATIC
    src/plugin_ui/plugin_ui.c
)
add_library(Umicom::plugin_ui ALIAS umicom_plugin_ui)
set_target_properties(umicom_plugin_ui PROPERTIES EXPORT_NAME plugin_ui)
target_link_libraries(umicom_plugin_ui PUBLIC
    Umicom::plugin
    Umicom::ui
)
target_include_directories(umicom_plugin_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_plugin_ui)
umicom_apply_sanitizers(umicom_plugin_ui)

# -----------------------------------------------------------------------------
# Project and repository scaffolding
# -----------------------------------------------------------------------------
add_library(umicom_scaffold STATIC
    src/scaffold/scaffold.c
    src/scaffold/repository.c
)
add_library(Umicom::scaffold ALIAS umicom_scaffold)
set_target_properties(umicom_scaffold PROPERTIES EXPORT_NAME scaffold)
target_link_libraries(umicom_scaffold PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::runtime
    Umicom::toolchain
    Umicom::repository
)
target_include_directories(umicom_scaffold PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_scaffold)
umicom_apply_sanitizers(umicom_scaffold)


# -----------------------------------------------------------------------------
# Native Web Server and HTTP application boundary
# -----------------------------------------------------------------------------
set(UMICOM_WEB_PLATFORM_SOURCE
    src/web/types.c
    src/web/header.c
    src/web/request.c
    src/web/response.c
    src/web/url.c
    src/web/query.c
    src/web/parser.c
    src/web/route.c
    src/web/router.c
    src/web/middleware.c
    src/web/mime.c
    src/web/static_files.c
    src/web/json.c
    src/web/rest.c
    src/web/sse.c
    src/web/websocket.c
    src/web/session.c
    src/web/origin.c
    src/web/security.c
    src/web/endpoint.c
    src/web/metrics.c
    src/web/server_config.c
    src/web/server_state.c
    src/web/listener.c
    src/web/service.c
    src/web/runtime.c
    src/web/server.c
)
if(WIN32)
    list(APPEND UMICOM_WEB_PLATFORM_SOURCE
        adapters/windows/web_listener_windows.c
    )
else()
    list(APPEND UMICOM_WEB_PLATFORM_SOURCE
        adapters/posix/web_listener_posix.c
    )
endif()

add_library(umicom_web STATIC ${UMICOM_WEB_PLATFORM_SOURCE})
add_library(Umicom::web ALIAS umicom_web)
set_target_properties(umicom_web PROPERTIES EXPORT_NAME web)
target_link_libraries(umicom_web PUBLIC
    Umicom::base
    Umicom::platform
)
if(WIN32)
    target_link_libraries(umicom_web PRIVATE ws2_32)
endif()
target_include_directories(umicom_web PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_web)
umicom_apply_sanitizers(umicom_web)

# -----------------------------------------------------------------------------
# Embedded-browser abstraction
# -----------------------------------------------------------------------------
add_library(umicom_browser STATIC
    src/browser/types.c
    src/browser/browser.c
    src/browser/navigation.c
    src/browser/bridge.c
    src/browser/headless.c
    src/browser/policy.c
)
add_library(Umicom::browser ALIAS umicom_browser)
set_target_properties(umicom_browser PROPERTIES EXPORT_NAME browser)
target_link_libraries(umicom_browser PUBLIC
    Umicom::base
    Umicom::web
)
target_include_directories(umicom_browser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_browser)
umicom_apply_sanitizers(umicom_browser)

# -----------------------------------------------------------------------------
# Toolkit-neutral real-time chart model
# -----------------------------------------------------------------------------
add_library(umicom_chart STATIC
    src/chart/types.c
    src/chart/candle.c
    src/chart/series.c
    src/chart/viewport.c
    src/chart/model.c
    src/chart/indicator.c
    src/chart/snapshot.c
    src/chart/json.c
    src/chart/pane.c
    src/chart/scale.c
    src/chart/crosshair.c
    src/chart/marker.c
    src/chart/annotation.c
    src/chart/drawing.c
    src/chart/stream.c
    src/chart/extension.c
    src/chart/workspace.c
)
add_library(Umicom::chart ALIAS umicom_chart)
set_target_properties(umicom_chart PROPERTIES EXPORT_NAME chart)
target_link_libraries(umicom_chart PUBLIC
    Umicom::base
    Umicom::web
)
target_include_directories(umicom_chart PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_chart)
umicom_apply_sanitizers(umicom_chart)

# -----------------------------------------------------------------------------
# Multi-frontend application plans and browser-project generators
# -----------------------------------------------------------------------------
add_library(umicom_frontend STATIC
    src/frontend/types.c
    src/frontend/dev_types.c
    src/frontend/page.c
    src/frontend/route.c
    src/frontend/asset.c
    src/frontend/plan.c
    src/frontend/renderer.c
    src/frontend/generator.c
    src/frontend/gtkjs.c
    src/frontend/headless.c
    src/frontend/widget_tree.c
    src/frontend/signal.c
    src/frontend/binding.c
    src/frontend/web_session.c
    src/frontend/transport.c
    src/frontend/render_tree.c
    src/frontend/web_style.c
    src/frontend/application.c
    src/frontend/device_registry.c
    src/frontend/document_store.c
    src/frontend/dev_server.c
    src/frontend/preview_session.c
    src/frontend/dom_inspector.c
    src/frontend/network_log.c
    src/frontend/console_log.c
    src/frontend/build_profile.c
    src/frontend/hot_reload.c
    src/frontend/quality_audit.c
    src/frontend/frontend_studio.c
)
add_library(Umicom::frontend ALIAS umicom_frontend)
set_target_properties(umicom_frontend PROPERTIES EXPORT_NAME frontend)
target_link_libraries(umicom_frontend PUBLIC
    Umicom::base
    Umicom::web
)
target_include_directories(umicom_frontend PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_frontend)
umicom_apply_sanitizers(umicom_frontend)

# -----------------------------------------------------------------------------
# Provider-neutral AI platform
# -----------------------------------------------------------------------------
add_library(umicom_ai STATIC
    src/ai/approval.c
    src/ai/authorengine.c
    src/ai/authorengine_service.c
    src/ai/chunk.c
    src/ai/chunker.c
    src/ai/citation.c
    src/ai/context.c
    src/ai/context_broker.c
    src/ai/coding_types.c
    src/ai/coding_context.c
    src/ai/coding_patch.c
    src/ai/coding_assistant.c
    src/ai/conversation.c
    src/ai/conversation_archive.c
    src/ai/dataset.c
    src/ai/document.c
    src/ai/embedding.c
    src/ai/evaluation.c
    src/ai/health.c
    src/ai/local_runtime.c
    src/ai/message.c
    src/ai/model.c
    src/ai/policy.c
    src/ai/privacy_policy.c
    src/ai/prompt.c
    src/ai/provider.c
    src/ai/provider_registry.c
    src/ai/rag.c
    src/ai/remote_provider.c
    src/ai/request.c
    src/ai/response.c
    src/ai/retrieval.c
    src/ai/runtime.c
    src/ai/runtime_catalogue.c
    src/ai/session.c
    src/ai/source.c
    src/ai/stream.c
    src/ai/token_budget.c
    src/ai/tool.c
    src/ai/tool_registry.c
    src/ai/types.c
    src/ai/vector_store.c
    # Batch 49: provider-neutral Native RAG and offline Knowledge Centre.
    src/knowledge/types.c
    src/knowledge/collection.c
    src/knowledge/source.c
    src/knowledge/chunker.c
    src/knowledge/embedding_provider.c
    src/knowledge/vector_index.c
    src/knowledge/catalogue.c
    src/knowledge/ingestion.c
    src/knowledge/retrieval.c
    src/knowledge/service.c
    src/knowledge/archive.c
)
add_library(Umicom::ai ALIAS umicom_ai)
set_target_properties(umicom_ai PROPERTIES EXPORT_NAME ai)
target_link_libraries(umicom_ai PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::security
    Umicom::diagnostics
)
if(UNIX)
    target_link_libraries(umicom_ai PUBLIC m)
endif()
target_include_directories(umicom_ai PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_ai)
umicom_apply_sanitizers(umicom_ai)

# Toolkit-neutral AuthorEngine projections.  The reusable integration service
# stays independent of the desktop/web frontend chosen by an application.
add_library(umicom_ai_ui STATIC
    src/ai_ui/authorengine_views.c
    src/ai_ui/coding_assistant_views.c
    src/knowledge_ui/views.c
)
add_library(Umicom::ai_ui ALIAS umicom_ai_ui)
set_target_properties(umicom_ai_ui PROPERTIES EXPORT_NAME ai_ui)
target_link_libraries(umicom_ai_ui PUBLIC
    Umicom::ai
    Umicom::ui
)
target_include_directories(umicom_ai_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_ai_ui)
umicom_apply_sanitizers(umicom_ai_ui)

# -----------------------------------------------------------------------------
# Governed Helix evolutionary runtime
# -----------------------------------------------------------------------------
add_library(umicom_helix STATIC
    src/helix/agent.c
    src/helix/agent_registry.c
    src/helix/approval.c
    src/helix/candidate.c
    src/helix/diagnosis.c
    src/helix/evidence.c
    src/helix/experiment.c
    src/helix/fitness.c
    src/helix/generation.c
    src/helix/memory.c
    src/helix/mutation.c
    src/helix/observation.c
    src/helix/plan.c
    src/helix/policy.c
    src/helix/provenance.c
    src/helix/release_gate.c
    src/helix/rollback.c
    src/helix/runtime.c
    src/helix/types.c
    src/helix/workflow.c
    src/helix/execution_types.c
    src/helix/action.c
    src/helix/operation.c
    src/helix/agent_runtime.c
    src/helix/approval_gate.c
    src/helix/journal.c
    src/helix/execution_adapter.c
    src/helix/retry_loop.c
    src/helix/candidate_set.c
    src/helix/compensation_plan.c
    src/helix/candidate_provenance.c
    src/helix/orchestrator.c
    src/helix/helix_ui.c
)
add_library(Umicom::helix ALIAS umicom_helix)
set_target_properties(umicom_helix PROPERTIES EXPORT_NAME helix)
target_link_libraries(umicom_helix PUBLIC
    Umicom::base
    Umicom::ai
    Umicom::runtime
    Umicom::security
    Umicom::ui
)
target_include_directories(umicom_helix PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_helix)
umicom_apply_sanitizers(umicom_helix)


# -----------------------------------------------------------------------------
# Application delivery, packaging, release, update and rollback platform
# -----------------------------------------------------------------------------
add_library(umicom_delivery STATIC
    src/delivery/types.c
    src/delivery/artifact.c
    src/delivery/artifact_set.c
    src/delivery/manifest.c
    src/delivery/build_evidence.c
    src/delivery/stage.c
    src/delivery/package.c
    src/delivery/package_provider.c
    src/delivery/directory_package.c
    src/delivery/checksum.c
    src/delivery/integrity.c
    src/delivery/signature.c
    src/delivery/signing.c
    src/delivery/sbom.c
    src/delivery/license_inventory.c
    src/delivery/provenance.c
    src/delivery/release.c
    src/delivery/release_channel.c
    src/delivery/release_registry.c
    src/delivery/publish.c
    src/delivery/update.c
    src/delivery/update_feed.c
    src/delivery/update_plan.c
    src/delivery/rollback.c
    src/delivery/generation.c
    src/delivery/install_plan.c
    src/delivery/install_state.c
    src/delivery/runtime_bundle.c
    src/delivery/dependency_scan.c
    src/delivery/smoke_test.c
    src/delivery/gate.c
    src/delivery/pipeline.c
    src/delivery/pipeline_state.c
    src/delivery/retention.c
    src/delivery/cleanup.c
    src/delivery/report.c
    src/delivery/policy.c
    src/delivery/inventory.c
    src/delivery/environment.c
    src/delivery/deployment.c
    src/delivery/health.c
    src/delivery/compatibility.c
    src/delivery/metadata.c
    src/delivery/store.c
    src/delivery/audit.c
    src/delivery/channel_registry.c
    src/delivery/rollback_history.c
    src/delivery/verification.c
    src/delivery/release_note.c
    src/delivery/platform_matrix.c
    src/delivery/artifact_index.c
    src/delivery/release_candidate.c
    src/delivery/promotion.c
    src/delivery/release_lock.c
    src/delivery/package_manifest.c
    src/delivery/runtime_dependency.c
    src/delivery/portable_bundle.c
    src/delivery/installer.c
    src/delivery/release_evidence.c
    src/delivery/update_channel.c
    src/delivery/upgrade_plan.c
    src/delivery/product_release.c
)
add_library(Umicom::delivery ALIAS umicom_delivery)
set_target_properties(umicom_delivery PROPERTIES EXPORT_NAME delivery)
target_link_libraries(umicom_delivery PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::build
    Umicom::testing
    Umicom::vcs
)
target_include_directories(umicom_delivery
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/delivery
)
umicom_apply_warnings(umicom_delivery)
umicom_apply_sanitizers(umicom_delivery)

# -----------------------------------------------------------------------------
# ABI hardening library
# -----------------------------------------------------------------------------
add_library(umicom_abi STATIC
    src/abi/version.c
    src/abi/hash.c
    src/abi/feature.c
    src/abi/platform.c
    src/abi/symbol.c
    src/abi/struct_layout.c
    src/abi/descriptor.c
    src/abi/contract.c
    src/abi/compatibility.c
    src/abi/catalogue.c
    src/abi/baseline.c
    src/abi/deprecation.c
    src/abi/binary.c
    src/abi/report.c
    src/abi/check.c
)
add_library(Umicom::abi ALIAS umicom_abi)
set_target_properties(umicom_abi PROPERTIES EXPORT_NAME abi)
target_link_libraries(umicom_abi PUBLIC Umicom::base)
target_include_directories(umicom_abi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_abi)
umicom_apply_sanitizers(umicom_abi)

# -----------------------------------------------------------------------------
# Installed SDK validation library
# -----------------------------------------------------------------------------
add_library(umicom_sdk STATIC
    src/sdk/component.c
    src/sdk/manifest.c
    src/sdk/package.c
    src/sdk/locator.c
    src/sdk/probe.c
    src/sdk/compatibility.c
    src/sdk/consumer.c
    src/sdk/conformance.c
    src/sdk/installation.c
    src/sdk/report.c
    src/sdk/profile.c
    src/sdk/target.c
    src/sdk/toolchain.c
    src/sdk/dependency.c
    src/sdk/evidence.c
    src/sdk/validator.c
    src/sdk/export_catalogue.c
    src/sdk/consumer_package.c
    src/sdk/export_plan.c
)
add_library(Umicom::sdk ALIAS umicom_sdk)
set_target_properties(umicom_sdk PROPERTIES EXPORT_NAME sdk)
target_link_libraries(umicom_sdk PUBLIC Umicom::base Umicom::platform Umicom::abi)
target_include_directories(umicom_sdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_sdk)
umicom_apply_sanitizers(umicom_sdk)

# -----------------------------------------------------------------------------
# Product composition and reference-product library
# -----------------------------------------------------------------------------
add_library(umicom_product STATIC
    src/product/feature.c
    src/product/capability.c
    src/product/frontend.c
    src/product/branding.c
    src/product/service.c
    src/product/descriptor.c
    src/product/profile.c
    src/product/registry.c
    src/product/suite.c
    src/product/compatibility.c
    src/product/reference.c
    src/product/catalogue.c
    src/product/validation.c
    src/product/bootstrap.c
    src/product/manifest.c
    src/product/launcher.c
    src/product/marketplace.c
    src/product/metadata_provider.c
    src/product/installation_state.c
    src/product/update_policy.c
    src/product/centre.c
)
add_library(Umicom::product ALIAS umicom_product)
set_target_properties(umicom_product PROPERTIES EXPORT_NAME product)
target_link_libraries(umicom_product PUBLIC Umicom::base Umicom::runtime)
target_include_directories(umicom_product PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_product)
umicom_apply_sanitizers(umicom_product)

# -----------------------------------------------------------------------------
# Framework-authoritative application portfolio and federation platform
# -----------------------------------------------------------------------------
add_library(umicom_application STATIC
    src/application/definition.c
    src/application/portfolio.c
    src/application/component_catalogue.c
    src/application/component/types.c
    src/application/component/contract.c
    src/application/component/registry.c
    src/application/component/factory.c
    src/application/component/instance.c
    src/application/component/selection.c
    src/application/component/layout.c
    src/application/component/bundle.c
    src/application/boundary_audit.c
    src/application/governance/types.c
    src/application/governance/evidence.c
    src/application/governance/policy.c
    src/application/governance/inventory.c
    src/application/governance/query.c
    src/application/governance/release_gate.c
    src/application/governance/report.c
    src/application/federation.c
    src/application/context_hub.c
    src/application/resource_broker.c
    src/application/master_authority.c
)
add_library(Umicom::application ALIAS umicom_application)
set_target_properties(umicom_application PROPERTIES EXPORT_NAME application)
target_link_libraries(umicom_application PUBLIC
    Umicom::base
    Umicom::integration
    Umicom::runtime
)
target_include_directories(umicom_application PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_application)
umicom_apply_sanitizers(umicom_application)

# Component governance inventory and release-evidence control plane.
if(UMICOM_BUILD_NATIVE_TOOL AND NOT TARGET umicom-component-inventory)
    add_executable(umicom-component-inventory
        tools/component_inventory_main.c)
    target_link_libraries(umicom-component-inventory PRIVATE
        Umicom::application)
    umicom_apply_warnings(umicom-component-inventory)
    umicom_apply_sanitizers(umicom-component-inventory)
    install(TARGETS umicom-component-inventory
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Declarative component composition proof for common and product bundles.
if(UMICOM_BUILD_NATIVE_TOOL AND NOT TARGET umicom-component-composer)
    add_executable(umicom-component-composer
        tools/component_composer_main.c)
    target_link_libraries(umicom-component-composer PRIVATE
        Umicom::application)
    umicom_apply_warnings(umicom-component-composer)
    umicom_apply_sanitizers(umicom-component-composer)
    install(TARGETS umicom-component-composer
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

add_library(umicom_application_ui STATIC
    src/application_ui/application_views.c
)
add_library(Umicom::application_ui ALIAS umicom_application_ui)
set_target_properties(umicom_application_ui PROPERTIES
    EXPORT_NAME application_ui)
target_link_libraries(umicom_application_ui PUBLIC
    Umicom::application
    Umicom::ui
    Umicom::ui_components
)
target_include_directories(umicom_application_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_application_ui)
umicom_apply_sanitizers(umicom_application_ui)

# -----------------------------------------------------------------------------
# Framework-owned federated desktop shell and layout runtime
# -----------------------------------------------------------------------------
add_library(umicom_desktop STATIC
    src/desktop/monitor_topology.c
    src/desktop/layout_tabs.c
    src/desktop/window_manager.c
    src/desktop/layout_catalogue.c
    src/desktop/context_links.c
    src/desktop/runtime.c
    src/desktop/content_runtime.c
    src/desktop/component_drag_drop.c
    src/desktop/context_synchronizer.c
    src/desktop/persistence.c
    src/desktop/layout_history.c
    src/desktop/session_recovery.c
    src/desktop/monitor_interaction.c
    src/desktop/ui_bridge.c
    src/desktop/master_authority.c
    src/desktop/monitor_canvas.c
    src/desktop/snap_engine.c
    src/desktop/layout_designer.c
    src/desktop/shell_actions.c
    src/desktop/shell_model.c
    src/desktop/views.c
)
add_library(Umicom::desktop ALIAS umicom_desktop)
set_target_properties(umicom_desktop PROPERTIES EXPORT_NAME desktop)
target_link_libraries(umicom_desktop PUBLIC
    Umicom::application
    Umicom::runtime
    Umicom::ui
    Umicom::ui_components
)
target_include_directories(umicom_desktop PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_desktop)
umicom_apply_sanitizers(umicom_desktop)
if(TARGET umicom_ui_gtk4)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::desktop)
endif()

# -----------------------------------------------------------------------------
# Product, marketplace and update distribution platform
# -----------------------------------------------------------------------------
# Distribution composes the existing Product Centre and Delivery contracts.
# Provider adapters may populate repositories, but generic policy, evidence,
# transaction and notification state remains reusable Framework functionality.
add_library(umicom_distribution STATIC
    src/distribution/package.c
    src/distribution/repository.c
    src/distribution/evidence.c
    src/distribution/policy.c
    src/distribution/transaction.c
    src/distribution/notification.c
    src/distribution/service.c
)
add_library(Umicom::distribution ALIAS umicom_distribution)
set_target_properties(umicom_distribution PROPERTIES EXPORT_NAME distribution)
target_link_libraries(umicom_distribution PUBLIC
    Umicom::base
    Umicom::delivery
    Umicom::product
)
target_include_directories(umicom_distribution PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_distribution)
umicom_apply_sanitizers(umicom_distribution)

# Toolkit-neutral Product Centre projections.  GTK4, Qt, Wt and headless
# frontends can render the same marketplace and update state.
add_library(umicom_distribution_ui STATIC
    src/distribution_ui/distribution_ui.c
)
add_library(Umicom::distribution_ui ALIAS umicom_distribution_ui)
set_target_properties(umicom_distribution_ui PROPERTIES EXPORT_NAME distribution_ui)
target_link_libraries(umicom_distribution_ui PUBLIC
    Umicom::distribution
    Umicom::ui
)
target_include_directories(umicom_distribution_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_distribution_ui)
umicom_apply_sanitizers(umicom_distribution_ui)

# -----------------------------------------------------------------------------
# CodeGuard security, vulnerability and architecture analysis
# -----------------------------------------------------------------------------
add_library(umicom_codeguard STATIC
    src/codeguard/architecture.c
    src/codeguard/category.c
    src/codeguard/config.c
    src/codeguard/duplicate.c
    src/codeguard/external_tool.c
    src/codeguard/lifetime.c
    src/codeguard/normalizer.c
    src/codeguard/pattern_scan.c
    src/codeguard/profile.c
    src/codeguard/quality_gate.c
    src/codeguard/report.c
    src/codeguard/result.c
    src/codeguard/rule_registry.c
    src/codeguard/rules/allocation.c
    src/codeguard/rules/arithmetic.c
    src/codeguard/rules/atomic.c
    src/codeguard/rules/buffer.c
    src/codeguard/rules/command.c
    src/codeguard/rules/concurrency.c
    src/codeguard/rules/coupling_rules.c
    src/codeguard/rules/database_boundary.c
    src/codeguard/rules/duplicate_rules.c
    src/codeguard/rules/environment.c
    src/codeguard/rules/error.c
    src/codeguard/rules/file.c
    src/codeguard/rules/format.c
    src/codeguard/rules/global_state.c
    src/codeguard/rules/input.c
    src/codeguard/rules/integer.c
    src/codeguard/rules/lifetime_rules.c
    src/codeguard/rules/lock.c
    src/codeguard/rules/logging.c
    src/codeguard/rules/memory.c
    src/codeguard/rules/network.c
    src/codeguard/rules/path.c
    src/codeguard/rules/pointer.c
    src/codeguard/rules/portability.c
    src/codeguard/rules/private_header.c
    src/codeguard/rules/process.c
    src/codeguard/rules/resource.c
    src/codeguard/rules/secret.c
    src/codeguard/rules/sql.c
    src/codeguard/rules/string.c
    src/codeguard/rules/temp_file.c
    src/codeguard/rules/ui_boundary.c
    src/codeguard/sanitise.c
    src/codeguard/scanner.c
    src/codeguard/service.c
    src/codeguard/severity.c
    src/codeguard/source.c
    src/codeguard/summary.c
    src/codeguard/suppression.c
    src/codeguard/quality_types.c
    src/codeguard/evidence.c
    src/codeguard/architecture_policy.c
    src/codeguard/unsafe_api.c
    src/codeguard/memory_audit.c
    src/codeguard/portability_audit.c
    src/codeguard/duplicate_evidence.c
    src/codeguard/api_baseline.c
    src/codeguard/quality_evaluation.c
    src/codeguard/remediation_plan.c
    src/codeguard/quality_session.c
    src/codeguard/quality_catalogue.c
)
add_library(Umicom::codeguard ALIAS umicom_codeguard)
set_target_properties(umicom_codeguard PROPERTIES EXPORT_NAME codeguard)
target_link_libraries(umicom_codeguard PUBLIC Umicom::base Umicom::platform)
target_include_directories(umicom_codeguard PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_codeguard)
umicom_apply_sanitizers(umicom_codeguard)
add_executable(umicom-codeguard tools/codeguard/main.c)
target_link_libraries(umicom-codeguard PRIVATE Umicom::codeguard)
umicom_apply_warnings(umicom-codeguard)
umicom_apply_sanitizers(umicom-codeguard)

# -----------------------------------------------------------------------------
# Reusable project and workspace model
# -----------------------------------------------------------------------------
add_library(umicom_project STATIC
    src/project/descriptor.c
    src/project/target.c
    src/project/configuration.c
    src/project/dependency.c
    src/project/file_set.c
    src/project/task.c
    src/project/launch_profile.c
    src/project/environment.c
    src/project/reference.c
    src/project/template.c
    src/project/variable.c
    src/project/capability.c
    src/project/build_node.c
    src/project/workspace.c
    # The pushed Batch 33 baseline keeps the import/validation implementation
    # co-located at the end of file_set.c and also retains workspace_import.c as
    # a complete source mirror. Compile the co-located implementation once;
    # compiling both files creates duplicate public symbols. No API, source
    # file, test or feature is removed by this compatibility correction.
    src/project/workspace_model.c
    src/project/workspace_group.c
    src/project/workspace_setting.c
    src/project/workspace_exclusion.c
    src/project/workspace_discovery.c
    src/project/workspace_refresh.c
    src/project/language_matrix.c
)
add_library(Umicom::project ALIAS umicom_project)
set_target_properties(umicom_project PROPERTIES EXPORT_NAME project)
target_link_libraries(umicom_project PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::build
)
target_include_directories(umicom_project PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_project)
umicom_apply_sanitizers(umicom_project)

# -----------------------------------------------------------------------------
# Provider-neutral language intelligence platform
# -----------------------------------------------------------------------------
add_library(umicom_language STATIC
    src/language/definition.c
    src/language/provider.c
    src/language/document.c
    src/language/symbol.c
    src/language/completion.c
    src/language/hover.c
    src/language/signature.c
    src/language/diagnostic.c
    src/language/code_action.c
    src/language/formatting.c
    src/language/reference.c
    src/language/rename.c
    src/language/semantic_token.c
    src/language/inlay_hint.c
    src/language/folding_range.c
    src/language/server_profile.c
    src/language/client_session.c
    src/language/request_ledger.c
    src/language/builtins.c
    src/language/profile.c
    src/language/problem_report.c
    src/language/navigation_history.c
    src/language/service.c
)
add_library(Umicom::language ALIAS umicom_language)
set_target_properties(umicom_language PROPERTIES EXPORT_NAME language)
target_link_libraries(umicom_language PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::protocol
    Umicom::toolchain
)
target_include_directories(umicom_language PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_language)
umicom_apply_sanitizers(umicom_language)

# -----------------------------------------------------------------------------
# Provider-neutral native debugger platform
# -----------------------------------------------------------------------------
add_library(umicom_debug STATIC
    src/debug/launch_configuration.c
    src/debug/breakpoint.c
    src/debug/session.c
    src/debug/thread.c
    src/debug/stack_frame.c
    src/debug/scope.c
    src/debug/variable.c
    src/debug/watch.c
    src/debug/console_entry.c
    src/debug/module.c
    src/debug/source.c
    src/debug/exception.c
    src/debug/event.c
    src/debug/adapter_profile.c
    src/debug/capabilities.c
    src/debug/controller.c
    src/debug/breakpoint_plan.c
    src/debug/timeline.c
    src/debug/view_state.c
    src/debug/watch_expression.c
    src/debug/workspace.c
    src/debug/service.c
    src/debug/configuration_resolver.c
    src/debug/compound_configuration.c
    src/debug/breakpoint_query.c
    src/debug/watch_query.c
    src/debug/console_query.c
    src/debug/command.c
    src/debug/orchestration.c
)
add_library(Umicom::debug ALIAS umicom_debug)
set_target_properties(umicom_debug PROPERTIES EXPORT_NAME debug)
target_link_libraries(umicom_debug PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::protocol
)
target_include_directories(umicom_debug PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_debug)
umicom_apply_sanitizers(umicom_debug)

# -----------------------------------------------------------------------------
# Source-control workbench platform over the existing VCS provider contracts
# -----------------------------------------------------------------------------
add_library(umicom_source_control STATIC
    src/source_control/repository.c
    src/source_control/change.c
    src/source_control/change_set.c
    src/source_control/staging.c
    src/source_control/commit.c
    src/source_control/branch.c
    src/source_control/tag.c
    src/source_control/remote.c
    src/source_control/diff_session.c
    src/source_control/operation.c
    src/source_control/history_entry.c
    src/source_control/service.c
)
add_library(Umicom::source_control ALIAS umicom_source_control)
set_target_properties(umicom_source_control PROPERTIES EXPORT_NAME source_control)
target_link_libraries(umicom_source_control PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::vcs
)
target_include_directories(umicom_source_control PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_source_control)
umicom_apply_sanitizers(umicom_source_control)

# -----------------------------------------------------------------------------
# Test discovery, execution, result and coverage workbench platform
# -----------------------------------------------------------------------------
add_library(umicom_test_platform STATIC
    src/test_platform/item.c
    src/test_platform/suite.c
    src/test_platform/run_profile.c
    src/test_platform/run_session.c
    src/test_platform/result.c
    src/test_platform/output.c
    src/test_platform/coverage.c
    src/test_platform/discovery.c
    src/test_platform/attachment.c
    src/test_platform/benchmark.c
    src/test_platform/filter.c
    src/test_platform/hierarchy.c
    src/test_platform/history.c
    src/test_platform/operation.c
    src/test_platform/artifact_contract.c
    src/test_platform/build_readiness/types.c
    src/test_platform/build_readiness/artifact.c
    src/test_platform/build_readiness/product_profile.c
    src/test_platform/build_readiness/catalogue.c
    src/test_platform/build_readiness/inventory.c
    src/test_platform/build_readiness/diagnostic.c
    src/test_platform/build_readiness/validation_plan.c
    src/test_platform/build_readiness/readiness_report.c
    src/test_platform/build_readiness/controller.c
    src/test_platform/build_readiness/command.c
    src/test_platform/ctest.c
    src/test_platform/structured_report.c
    src/test_platform/execution.c
    src/test_platform/service.c
    src/test_platform/workspace.c
)
add_library(Umicom::test_platform ALIAS umicom_test_platform)
set_target_properties(umicom_test_platform PROPERTIES EXPORT_NAME test_platform)
target_link_libraries(umicom_test_platform PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::testing
)
target_include_directories(umicom_test_platform PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_test_platform)
umicom_apply_sanitizers(umicom_test_platform)

# Framework-owned Test Explorer presentation shared by Studio and every future
# application.  The factories expose ordinary toolkit-neutral view models.
add_library(umicom_test_ui STATIC
    src/test_ui/test_views.c
)
add_library(Umicom::test_ui ALIAS umicom_test_ui)
set_target_properties(umicom_test_ui PROPERTIES EXPORT_NAME test_ui)
target_link_libraries(umicom_test_ui PUBLIC
    Umicom::test_platform
    Umicom::ui
)
target_include_directories(umicom_test_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_test_ui)
umicom_apply_sanitizers(umicom_test_ui)


# -----------------------------------------------------------------------------
# Reusable developer runtime orchestration
# -----------------------------------------------------------------------------
# The developer runtime coordinates the project, language, debugger,
# source-control, test and workbench services through a deterministic operation
# pipeline.  Applications can bind existing service instances (as Studio does)
# or let the runtime own a complete standalone service set.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomDeveloperCapabilities.cmake")

set(UMICOM_DEVELOPER_ASSEMBLY_SOURCE "")
set(UMICOM_DEVELOPER_ASSEMBLY_DEFINITION "")
if(UMICOM_DEVELOPER_ENABLE_ASSEMBLY AND
   CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND
   CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
    enable_language(ASM)
    set(UMICOM_DEVELOPER_ASSEMBLY_SOURCE
        src/developer/asm/spin_hint_x86_64.S)
    set(UMICOM_DEVELOPER_ASSEMBLY_DEFINITION
        UMICOM_DEVELOPER_SPIN_X86_64_ASM=1)
endif()

add_library(umicom_developer STATIC
    src/developer/capability.c
    src/developer/capability_builtin.c
    src/developer/spin_hint.c
    src/developer/types.c
    src/developer/context.c
    src/developer/operation.c
    src/developer/pipeline.c
    src/developer/journal.c
    src/developer/executor.c
    src/developer/cmake_plan.c
    src/developer/runtime.c
    src/developer/command_line.c
    src/developer/task_plan.c
    src/developer/launch_plan.c
    src/developer/workflow.c
    src/developer/batch_execution.c
    src/developer/workspace_state.c
    src/developer/project_bootstrap.c
    src/developer/toolchain_binding.c
    src/developer/universal_model.c
    ${UMICOM_DEVELOPER_CAPABILITY_SOURCES}
    ${UMICOM_DEVELOPER_ASSEMBLY_SOURCE}
)
add_library(Umicom::developer ALIAS umicom_developer)
set_target_properties(umicom_developer PROPERTIES EXPORT_NAME developer)
target_link_libraries(umicom_developer PUBLIC
    Umicom::base
    Umicom::platform
    Umicom::ui
    Umicom::project
    Umicom::language
    Umicom::debug
    Umicom::source_control
    Umicom::test_platform
    Umicom::toolchain
)
target_include_directories(umicom_developer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(UMICOM_DEVELOPER_ASSEMBLY_DEFINITION)
    target_compile_definitions(umicom_developer PRIVATE
        ${UMICOM_DEVELOPER_ASSEMBLY_DEFINITION})
endif()
umicom_apply_warnings(umicom_developer)
umicom_apply_sanitizers(umicom_developer)

# -----------------------------------------------------------------------------
# Reusable financial foundation
# -----------------------------------------------------------------------------
add_library(umicom_finance STATIC
    src/finance/identifier.c
    src/finance/currency.c
    src/finance/money.c
    src/finance/quantity.c
    src/finance/account.c
    src/finance/party.c
    src/finance/business_time.c
    src/finance/decimal.c
)
add_library(Umicom::finance ALIAS umicom_finance)
set_target_properties(umicom_finance PROPERTIES EXPORT_NAME finance)
target_link_libraries(umicom_finance PUBLIC Umicom::base)
target_include_directories(umicom_finance PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_finance)
umicom_apply_sanitizers(umicom_finance)

# -----------------------------------------------------------------------------
# Reusable trading, risk, replay and market-research foundation
# -----------------------------------------------------------------------------
add_library(umicom_trading STATIC
    src/trading/alert.c
    src/trading/bar.c
    src/trading/broker.c
    src/trading/buying_power.c
    src/trading/cash.c
    src/trading/depth.c
    src/trading/environment.c
    src/trading/execution_report.c
    src/trading/execution_store.c
    src/trading/factor_attribution.c
    src/trading/factor_observation.c
    src/trading/feature_vector.c
    src/trading/fee.c
    src/trading/fill.c
    src/trading/fix_boundary.c
    src/trading/health.c
    src/trading/ibkr_boundary.c
    src/trading/instrument.c
    src/trading/journal_digest.c
    src/trading/kill_switch.c
    src/trading/market_data_quality.c
    src/trading/market_data_snapshot.c
    src/trading/market_gap.c
    src/trading/market_replay.c
    src/trading/market_session.c
    src/trading/market_state.c
    src/trading/mean_reversion.c
    src/trading/momentum.c
    src/trading/movement_event.c
    src/trading/oms.c
    src/trading/order_book.c
    src/trading/order_request.c
    src/trading/order_state.c
    src/trading/order_store.c
    src/trading/order_throttle.c
    src/trading/order_type.c
    src/trading/order_validation.c
    src/trading/pnl.c
    src/trading/portfolio.c
    src/trading/position.c
    src/trading/position_book.c
    src/trading/pretrade_risk.c
    src/trading/price_band.c
    src/trading/quote.c
    src/trading/replay_clock.c
    src/trading/replay_cursor.c
    src/trading/replay_event.c
    src/trading/research_stats.c
    src/trading/research_window.c
    src/trading/risk_context.c
    src/trading/risk_decision.c
    src/trading/risk_limit.c
    src/trading/self_trade.c
    src/trading/session_calendar.c
    src/trading/simulated_broker.c
    src/trading/strategy_signal.c
    src/trading/support_resistance.c
    src/trading/surveillance.c
    src/trading/tick.c
    src/trading/time_in_force.c
    src/trading/trade_capture.c
    src/trading/trading_profile.c
    src/trading/venue.c
    src/trading/venue_adapter.c
    src/trading/volatility_proxy.c
    src/trading/watchlist.c
    src/trading/workspace.c
)
add_library(Umicom::trading ALIAS umicom_trading)
set_target_properties(umicom_trading PROPERTIES EXPORT_NAME trading)
target_link_libraries(umicom_trading PUBLIC
    Umicom::base
    Umicom::chart
    Umicom::finance
)
target_include_directories(umicom_trading PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_trading)
umicom_apply_sanitizers(umicom_trading)

# Framework-owned professional trading presentation. The view layer links the
# coordinator to semantic Umicom UI models while remaining reusable by Studio,
# Trader, TMS and every supported frontend adapter.
add_library(umicom_trading_ui STATIC
    src/trading_ui/trading_views.c
)
add_library(Umicom::trading_ui ALIAS umicom_trading_ui)
set_target_properties(umicom_trading_ui PROPERTIES EXPORT_NAME trading_ui)
target_link_libraries(umicom_trading_ui PUBLIC
    Umicom::trading
    Umicom::ui
)
target_include_directories(umicom_trading_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_trading_ui)
umicom_apply_sanitizers(umicom_trading_ui)

# The aggregate Framework target lets applications consume the complete
# foundation with one target: Umicom::Framework.
add_library(umicom_framework INTERFACE)
add_library(Umicom::Framework ALIAS umicom_framework)
set_target_properties(umicom_framework PROPERTIES EXPORT_NAME Framework)
target_link_libraries(umicom_framework INTERFACE
    Umicom::base
    Umicom::diagnostics
    Umicom::messaging
    Umicom::data
    Umicom::platform
    Umicom::security
    Umicom::runtime
    Umicom::integration
    Umicom::resilience
    Umicom::observability
    Umicom::toolchain
    Umicom::compiler
    Umicom::repository
    Umicom::plugin
    Umicom::plugin_ui
    Umicom::scaffold
    Umicom::ui
    Umicom::ui_components
    Umicom::ui_headless
    Umicom::build
    Umicom::build_ui
    Umicom::diagnostic_ui
    Umicom::testing
    Umicom::terminal
    Umicom::terminal_ui
    Umicom::protocol
    Umicom::editor
    Umicom::document
    Umicom::vcs
    Umicom::vcs_ui
    Umicom::declarative
    Umicom::designer
    Umicom::web
    Umicom::browser
    Umicom::chart
    Umicom::frontend
    Umicom::ai
    Umicom::ai_ui
    Umicom::helix
    Umicom::delivery
    Umicom::abi
    Umicom::sdk
    Umicom::product
    Umicom::application
    Umicom::application_ui
    Umicom::desktop
    Umicom::distribution
    Umicom::distribution_ui
    Umicom::codeguard
    Umicom::project
    Umicom::language
    Umicom::debug
    Umicom::source_control
    Umicom::test_platform
    Umicom::test_ui
    Umicom::developer
    Umicom::finance
    Umicom::trading
    Umicom::trading_ui
)

# -----------------------------------------------------------------------------
# Native Umicom command
# -----------------------------------------------------------------------------
if(UMICOM_BUILD_NATIVE_TOOL)
    add_executable(umicom
        tools/umicom/src/main.c
        tools/umicom/src/common.c
        tools/umicom/src/command_check.c
        tools/umicom/src/command_build.c
        tools/umicom/src/command_repository.c
        tools/umicom/src/command_catalogue.c
        tools/umicom/src/command_security.c
    )
    target_include_directories(umicom PRIVATE tools/umicom/src)
    target_compile_definitions(umicom PRIVATE
        UMICOM_REPOSITORY_TEMPLATE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/templates/repository"
    )
    target_link_libraries(umicom PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom)
    umicom_apply_sanitizers(umicom)
endif()

# -----------------------------------------------------------------------------
# Demonstration and tests
# -----------------------------------------------------------------------------
add_executable(umicom-framework-demo
    examples/framework_demo/main.c
)
target_link_libraries(umicom-framework-demo PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-framework-demo)
umicom_apply_sanitizers(umicom-framework-demo)
include(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomApplicationExperienceIntegration.cmake"
)
include(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomApplicationRuntimeIntegration.cmake"
)
if(BUILD_TESTING)
    function(umicom_add_framework_test target test_name source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE Umicom::Framework)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        add_test(NAME ${test_name} COMMAND ${target})
    endfunction()
    umicom_add_framework_test(umicom-base-text-test framework.base_text tests/test_base_text.c)
    # Batch 58: unified compiler providers, polyglot ABI/build graphs and Umicc.
    umicom_add_framework_test(umicom-compiler-language-target-test framework.compiler.language_target tests/test_compiler_language_target.c)
    umicom_add_framework_test(umicom-compiler-provider-registry-test framework.compiler.provider_registry tests/test_compiler_provider_registry.c)
    umicom_add_framework_test(umicom-compiler-request-plan-test framework.compiler.request_plan tests/test_compiler_request_plan.c)
    umicom_add_framework_test(umicom-compiler-diagnostic-test framework.compiler.diagnostic tests/test_compiler_diagnostic.c)
    umicom_add_framework_test(umicom-compiler-interop-graph-test framework.compiler.interop_graph tests/test_compiler_interop_graph.c)
    umicom_add_framework_test(umicom-compiler-database-lock-test framework.compiler.database_lock tests/test_compiler_database_lock.c)
    umicom_add_framework_test(umicom-compiler-cache-runtime-test framework.compiler.cache_runtime tests/test_compiler_cache_runtime.c)
    umicom_add_framework_test(umicom-compiler-centre-test framework.compiler.centre tests/test_compiler_centre.c)
    umicom_add_framework_test(umicom-compiler-build-graph-test framework.compiler.build_graph tests/test_compiler_build_graph.c)
    umicom_add_framework_test(umicom-compiler-response-database-test framework.compiler.response_database tests/test_compiler_response_database.c)
    umicom_add_framework_test(umicom-compiler-policy-profile-test framework.compiler.policy_profile tests/test_compiler_policy_profile.c)
    umicom_add_framework_test(umicom-compiler-sysroot-runtime-test framework.compiler.sysroot_runtime tests/test_compiler_sysroot_runtime.c)

    # Batch 57: observability, resilience evidence and professional workspaces.
    umicom_add_framework_test(umicom-operations-histories-test framework.operations_histories tests/test_operations_histories.c)
    umicom_add_framework_test(umicom-operations-centre-test framework.operations_centre tests/test_operations_centre.c)
    umicom_add_framework_test(umicom-operations-catalogue-test framework.operations_catalogue tests/test_operations_catalogue.c)
    umicom_add_framework_test(umicom-operations-health-test framework.operations_health tests/test_operations_health.c)
    umicom_add_framework_test(umicom-operations-correlation-test framework.operations_correlation tests/test_operations_correlation.c)
    umicom_add_framework_test(umicom-operations-resilience-test framework.operations_resilience tests/test_operations_resilience.c)
    umicom_add_framework_test(umicom-workspace-layout-test framework.workspace_layout tests/test_workspace_layout.c)
    umicom_add_framework_test(umicom-window-catalogue-group-test framework.window_catalogue_group tests/test_window_catalogue_group.c)
    umicom_add_framework_test(umicom-layout-library-theme-test framework.layout_library_theme tests/test_layout_library_theme.c)
    umicom_add_framework_test(umicom-layout-persistence-test framework.layout_persistence tests/test_layout_persistence.c)
    umicom_add_framework_test(umicom-workspace-customisation-test framework.workspace_customisation tests/test_workspace_customisation.c)

    # Unified immutable diagnostics, Problems and Output pipeline.
    umicom_add_framework_test(umicom-diagnostic-snapshot-test framework.diagnostic_snapshot tests/test_diagnostic_snapshot.c)
    umicom_add_framework_test(umicom-diagnostic-model-test framework.diagnostic_model tests/test_diagnostic_model.c)
    umicom_add_framework_test(umicom-diagnostic-filter-test framework.diagnostic_filter tests/test_diagnostic_filter.c)
    umicom_add_framework_test(umicom-diagnostic-query-test framework.diagnostic_query tests/test_diagnostic_query.c)
    umicom_add_framework_test(umicom-output-buffer-test framework.output_buffer tests/test_output_buffer.c)
    umicom_add_framework_test(umicom-output-filter-test framework.output_filter tests/test_output_filter.c)
    umicom_add_framework_test(umicom-compiler-diagnostic-parser-test framework.compiler_diagnostic_parser tests/test_compiler_diagnostic_parser.c)
    umicom_add_framework_test(umicom-runtime-diagnostic-parser-test framework.runtime_diagnostic_parser tests/test_runtime_diagnostic_parser.c)
    umicom_add_framework_test(umicom-diagnostic-parser-registry-test framework.diagnostic_parser_registry tests/test_diagnostic_parser_registry.c)
    umicom_add_framework_test(umicom-diagnostic-event-test framework.diagnostic_event tests/test_diagnostic_event.c)
    umicom_add_framework_test(umicom-diagnostic-pipeline-test framework.diagnostic_pipeline tests/test_diagnostic_pipeline.c)
    umicom_add_framework_test(umicom-diagnostic-ui-projection-test framework.diagnostic_ui_projection tests/test_diagnostic_ui_projection.c)
    umicom_add_framework_test(umicom-diagnostic-build-projection-test framework.diagnostic_build_projection tests/test_diagnostic_build_projection.c)
    umicom_add_framework_test(umicom-diagnostic-ui-views-test framework.diagnostic_ui_views tests/test_diagnostic_ui_views.c)
    umicom_add_framework_test(umicom-diagnostic-navigation-test framework.diagnostic_navigation tests/test_diagnostic_navigation.c)

    # Unified workbench and major reusable platform expansion.
    umicom_add_framework_test(umicom-ui-activity-tests framework.ui.activity tests/test_ui_activity.c)
    umicom_add_framework_test(umicom-ui-view-container-tests framework.ui.view_container tests/test_ui_view_container.c)
    umicom_add_framework_test(umicom-ui-context-tests framework.ui.context tests/test_ui_context.c)
    umicom_add_framework_test(umicom-ui-keybinding-tests framework.ui.keybinding tests/test_ui_keybinding.c)
    umicom_add_framework_test(umicom-ui-quick-access-tests framework.ui.quick_access tests/test_ui_quick_access.c)
    umicom_add_framework_test(umicom-ui-breadcrumb-tests framework.ui.breadcrumb tests/test_ui_breadcrumb.c)
    umicom_add_framework_test(umicom-ui-explorer-tests framework.ui.explorer tests/test_ui_explorer.c)
    umicom_add_framework_test(umicom-ui-workbench-state-tests framework.ui.workbench_state tests/test_ui_workbench_state.c)
    umicom_add_framework_test(umicom-ui-document-layout-tests framework.ui.document_layout tests/test_ui_document_layout.c)
    umicom_add_framework_test(umicom-ui-workspace-profile-tests framework.ui.workspace_profile tests/test_ui_workspace_profile.c)
    umicom_add_framework_test(umicom-ui-workspace-profile-persistence-tests framework.ui.workspace_profile_persistence tests/test_ui_workspace_profile_persistence.c)
    umicom_add_framework_test(umicom-ui-workspace-profile-designer-tests framework.ui.workspace_profile_designer tests/test_ui_workspace_profile_designer.c)
    umicom_add_framework_test(umicom-ui-appearance-tests framework.ui.appearance tests/test_ui_appearance.c)
    umicom_add_framework_test(umicom-ui-appearance-persistence-tests framework.ui.appearance_persistence tests/test_ui_appearance_persistence.c)
    umicom_add_framework_test(umicom-ui-view-presentation-tests framework.ui.view_presentation tests/test_ui_view_presentation.c)
    umicom_add_framework_test(umicom-ui-headless-view-presentation-tests framework.ui.headless_view_presentation tests/test_ui_headless_view_presentation.c)
    umicom_add_framework_test(umicom-ui-platform-models-tests framework.ui.platform_models tests/test_ui_platform_models.c)
    umicom_add_framework_test(umicom-resource-navigation-tests framework.resource_navigation tests/test_resource_navigation.c)
    umicom_add_framework_test(umicom-frontend-platform-tests framework.frontend_platform tests/test_frontend_platform.c)
    umicom_add_framework_test(umicom-product-marketplace-tests framework.product_marketplace tests/test_product_marketplace.c)
    umicom_add_framework_test(umicom-chart-interaction-tests framework.chart_interaction tests/test_chart_interaction.c)
    umicom_add_framework_test(umicom-designer-authoring-tools-tests framework.designer_authoring_tools tests/test_designer_authoring_tools.c)

    # Batch 26 developer-platform integration now runs through the real
    # Framework build rather than only through isolated source compilation.
    umicom_add_framework_test(umicom-project-workspace-tests framework.project_workspace tests/test_project_workspace.c)
    umicom_add_framework_test(umicom-language-service-platform-tests framework.language_service_platform tests/test_language_service.c)
    umicom_add_framework_test(umicom-debug-service-platform-tests framework.debug_service_platform tests/test_debug_service.c)
    # Batch 41 language-intelligence and debugger protocol/runtime coverage.
    foreach(UMICOM_LANGUAGE_DEBUG_TEST
            lsp_extended
            dap_extended
            compilation_database
            language_server_profile
            language_client_session
            language_request_ledger
            language_builtins
            language_problem_report
            language_navigation_history
            debug_adapter_profile
            debug_capabilities
            debug_controller
            debug_breakpoint_plan
            debug_timeline
            debug_view_state
            debug_watch_expression
            debug_workspace)
        string(REPLACE "_" "-" UMICOM_LANGUAGE_DEBUG_TARGET
            "${UMICOM_LANGUAGE_DEBUG_TEST}")
        umicom_add_framework_test(
            umicom-${UMICOM_LANGUAGE_DEBUG_TARGET}-tests
            framework.${UMICOM_LANGUAGE_DEBUG_TEST}
            tests/test_${UMICOM_LANGUAGE_DEBUG_TEST}.c)
    endforeach()
    umicom_add_framework_test(umicom-debug-configuration-resolver-tests framework.debug_configuration_resolver tests/test_debug_configuration_resolver.c)
    umicom_add_framework_test(umicom-debug-compound-configuration-tests framework.debug_compound_configuration tests/test_debug_compound_configuration.c)
    umicom_add_framework_test(umicom-debug-breakpoint-query-tests framework.debug_breakpoint_query tests/test_debug_breakpoint_query.c)
    umicom_add_framework_test(umicom-debug-watch-query-tests framework.debug_watch_query tests/test_debug_watch_query.c)
    umicom_add_framework_test(umicom-debug-console-query-tests framework.debug_console_query tests/test_debug_console_query.c)
    umicom_add_framework_test(umicom-debug-command-tests framework.debug_commands tests/test_debug_command.c)
    umicom_add_framework_test(umicom-debug-orchestration-tests framework.debug_orchestration tests/test_debug_orchestration.c)
    umicom_add_framework_test(umicom-source-control-service-platform-tests framework.source_control_service_platform tests/test_source_control_service_platform.c)
    umicom_add_framework_test(umicom-source-control-workspace-bridge-tests framework.source_control_workspace_bridge tests/test_source_control_workspace_bridge.c)
    umicom_add_framework_test(umicom-test-platform-service-tests framework.test_platform_service tests/test_test_platform_service.c)
    # Batch 37 Test Platform v2: metadata discovery, filters, hierarchy,
    # operation control, retained history and tool-neutral artifact contracts.
    umicom_add_framework_test(umicom-test-platform-ctest-tests framework.test_platform.ctest tests/test_test_platform_ctest.c)
    umicom_add_framework_test(umicom-test-platform-filter-history-tests framework.test_platform.filter_history tests/test_test_platform_filter_history.c)
    umicom_add_framework_test(umicom-test-platform-hierarchy-tests framework.test_platform.hierarchy tests/test_test_platform_hierarchy.c)
    umicom_add_framework_test(umicom-test-platform-operation-tests framework.test_platform.operation tests/test_test_platform_operation.c)
    umicom_add_framework_test(umicom-test-platform-artifact-tests framework.test_platform.artifact tests/test_test_platform_artifact.c)
    umicom_add_framework_test(umicom-test-platform-structured-report-tests framework.test_platform.structured_report tests/test_test_platform_structured_report.c)
    umicom_add_framework_test(umicom-test-platform-execution-tests framework.test_platform.execution tests/test_test_platform_execution.c)
    # Build readiness closes the gap between CTest registration and executable
    # production, so Not Run results report their real pre-test cause.
    umicom_add_framework_test(umicom-test-build-readiness-types-test framework.test_platform.build_readiness.types tests/test_platform_build_readiness/test_build_readiness_types.c)
    umicom_add_framework_test(umicom-test-build-readiness-artifact-test framework.test_platform.build_readiness.artifact tests/test_platform_build_readiness/test_build_readiness_artifact.c)
    umicom_add_framework_test(umicom-test-build-readiness-product-profile-test framework.test_platform.build_readiness.product_profile tests/test_platform_build_readiness/test_build_readiness_product_profile.c)
    umicom_add_framework_test(umicom-test-build-readiness-catalogue-test framework.test_platform.build_readiness.catalogue tests/test_platform_build_readiness/test_build_readiness_catalogue.c)
    umicom_add_framework_test(umicom-test-build-readiness-inventory-test framework.test_platform.build_readiness.inventory tests/test_platform_build_readiness/test_build_readiness_inventory.c)
    umicom_add_framework_test(umicom-test-build-readiness-diagnostic-test framework.test_platform.build_readiness.diagnostic tests/test_platform_build_readiness/test_build_readiness_diagnostic.c)
    umicom_add_framework_test(umicom-test-build-readiness-validation-plan-test framework.test_platform.build_readiness.validation_plan tests/test_platform_build_readiness/test_build_readiness_validation_plan.c)
    umicom_add_framework_test(umicom-test-build-readiness-report-test framework.test_platform.build_readiness.report tests/test_platform_build_readiness/test_build_readiness_report.c)
    umicom_add_framework_test(umicom-test-build-readiness-controller-test framework.test_platform.build_readiness.controller tests/test_platform_build_readiness/test_build_readiness_controller.c)
    umicom_add_framework_test(umicom-test-build-readiness-command-test framework.test_platform.build_readiness.command tests/test_platform_build_readiness/test_build_readiness_command.c)
    umicom_add_framework_test(umicom-test-workspace-tests framework.test_workspace tests/test_test_workspace.c)
    umicom_add_framework_test(umicom-test-ui-views-tests framework.test_ui_views tests/test_test_ui_views.c)
    umicom_add_framework_test(umicom-ui-workbench-services-tests framework.ui.workbench_services tests/test_ui_workbench_services.c)

    # Developer runtime orchestration, dependency scheduling and execution.
    umicom_add_framework_test(umicom-developer-context-tests framework.developer.context tests/test_developer_context.c)
    umicom_add_framework_test(umicom-developer-pipeline-tests framework.developer.pipeline tests/test_developer_pipeline.c)
    umicom_add_framework_test(umicom-developer-journal-tests framework.developer.journal tests/test_developer_journal.c)
    umicom_add_framework_test(umicom-developer-cmake-plan-tests framework.developer.cmake_plan tests/test_developer_cmake_plan.c)
    umicom_add_framework_test(umicom-developer-runtime-tests framework.developer.runtime tests/test_developer_runtime.c)
    umicom_add_framework_test(umicom-language-profile-tests framework.language_profile tests/test_language_profile.c)
    umicom_add_framework_test(umicom-project-language-matrix-tests framework.project_language_matrix tests/test_project_language_matrix.c)
    umicom_add_framework_test(umicom-developer-toolchain-binding-tests framework.developer_toolchain_binding tests/test_developer_toolchain_binding.c)
    umicom_add_framework_test(umicom-developer-universal-model-tests framework.developer_universal_model tests/test_developer_universal_model.c)
    umicom_add_framework_test(umicom-developer-command-line-tests framework.developer.command_line tests/test_developer_command_line.c)
    umicom_add_framework_test(umicom-developer-task-plan-tests framework.developer.task_plan tests/test_developer_task_plan.c)
    umicom_add_framework_test(umicom-developer-launch-plan-tests framework.developer.launch_plan tests/test_developer_launch_plan.c)
    umicom_add_framework_test(umicom-developer-workflow-tests framework.developer.workflow tests/test_developer_workflow.c)
    umicom_add_framework_test(umicom-developer-batch-execution-tests framework.developer.batch_execution tests/test_developer_batch_execution.c)
    umicom_add_framework_test(umicom-developer-workspace-state-tests framework.developer.workspace_state tests/test_developer_workspace_state.c)

    # Filesystem project import/bootstrap and aggregate-contract regression coverage.
    # These tests extend the existing project/developer tests; they do not replace them.
    umicom_add_framework_test(umicom-project-workspace-import-tests framework.project_workspace_import tests/test_project_workspace_import.c)
    # Batch 34 extends the existing project platform with multi-root models,
    # inheritance, dependency ordering and non-destructive refresh planning.
    umicom_add_framework_test(umicom-project-workspace-model-tests framework.project_workspace_model tests/test_project_workspace_model.c)
    umicom_add_framework_test(umicom-project-workspace-inheritance-tests framework.project_workspace_inheritance tests/test_project_workspace_inheritance.c)
    umicom_add_framework_test(umicom-project-workspace-discovery-tests framework.project_workspace_discovery tests/test_project_workspace_discovery.c)
    umicom_add_framework_test(umicom-project-workspace-refresh-tests framework.project_workspace_refresh tests/test_project_workspace_refresh.c)
    umicom_add_framework_test(umicom-developer-project-bootstrap-tests framework.developer.project_bootstrap tests/test_developer_project_bootstrap.c)
    umicom_add_framework_test(umicom-aggregate-public-api-tests framework.aggregate_public_api tests/test_aggregate_public_api.c)

    # Runtime inventory, ABI/version contract and compatibility regression shield.
    # These tests are additive and intentionally preserve all existing coverage.
    umicom_add_framework_test(umicom-runtime-inventory-tests framework.runtime.inventory tests/test_runtime_inventory.c)
    umicom_add_framework_test(umicom-runtime-inventory-queries-tests framework.runtime.inventory_queries tests/test_runtime_inventory_queries.c)
    umicom_add_framework_test(umicom-runtime-inventory-partial-tests framework.runtime.inventory_partial tests/test_runtime_inventory_partial.c)
    umicom_add_framework_test(umicom-runtime-inventory-invalid-arguments-tests framework.runtime.inventory_invalid_arguments tests/test_runtime_inventory_invalid_arguments.c)
    umicom_add_framework_test(umicom-runtime-inventory-flag-matrix-tests framework.runtime.inventory_flag_matrix tests/test_runtime_inventory_flag_matrix.c)
    umicom_add_framework_test(umicom-runtime-contract-version-tests framework.runtime.contract_version tests/test_runtime_contract_version.c)
    umicom_add_framework_test(umicom-runtime-contract-abi-tests framework.runtime.contract_abi tests/test_runtime_contract_abi.c)
    umicom_add_framework_test(umicom-runtime-contract-lifecycle-tests framework.runtime.contract_lifecycle tests/test_runtime_contract_lifecycle.c)
    umicom_add_framework_test(umicom-runtime-contract-invalid-tests framework.runtime.contract_invalid tests/test_runtime_contract_invalid.c)
    umicom_add_framework_test(umicom-runtime-legacy-command-descriptor-tests framework.runtime.legacy_command_descriptor tests/test_runtime_legacy_command_descriptor.c)
    umicom_add_framework_test(umicom-runtime-legacy-service-descriptor-tests framework.runtime.legacy_service_descriptor tests/test_runtime_legacy_service_descriptor.c)
    umicom_add_framework_test(umicom-runtime-legacy-capability-descriptor-tests framework.runtime.legacy_capability_descriptor tests/test_runtime_legacy_capability_descriptor.c)
    umicom_add_framework_test(umicom-runtime-contract-registry-integration-tests framework.runtime.contract_registry_integration tests/test_runtime_contract_registry_integration.c)
    umicom_add_framework_test(umicom-aggregate-header-contract-tests framework.aggregate_header_contract tests/test_aggregate_header_contract.c)
    umicom_add_framework_test(
        umicom-version-consistency-tests
        framework.version_consistency
        tests/test_version_consistency.c
    )
    target_compile_definitions(umicom-version-consistency-tests PRIVATE
        UMICOM_FRAMEWORK_PROJECT_VERSION="${PROJECT_VERSION}"
        UMICOM_FRAMEWORK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )

    umicom_add_framework_test(
        umicom-framework-tests
        framework.foundation
        tests/test_framework.c
    )

    add_executable(umicom-scaffold-tests tests/test_scaffold.c)
    target_link_libraries(umicom-scaffold-tests PRIVATE Umicom::Framework)
    target_compile_definitions(umicom-scaffold-tests PRIVATE
        UMICOM_TEST_TEMPLATE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/templates"
    )
    umicom_apply_warnings(umicom-scaffold-tests)
    umicom_apply_sanitizers(umicom-scaffold-tests)
    add_test(NAME framework.scaffold COMMAND umicom-scaffold-tests)

    umicom_add_framework_test(
        umicom-diagnostic-hub-tests
        framework.diagnostic_hub
        tests/test_diagnostic_hub.c
    )
    umicom_add_framework_test(
        umicom-clock-tests
        framework.clock
        tests/test_clock.c
    )
    umicom_add_framework_test(
        umicom-diagnostic-store-tests
        framework.diagnostic_store
        tests/test_diagnostic_store.c
    )
    umicom_add_framework_test(
        umicom-settings-tests
        framework.settings
        tests/test_settings.c
    )
    umicom_add_framework_test(
        umicom-capability-registry-tests
        framework.capability_registry
        tests/test_capability_registry.c
    )
    umicom_add_framework_test(
        umicom-application-manifest-tests
        framework.application_manifest
        tests/test_application_manifest.c
    )
    umicom_add_framework_test(
        umicom-suite-tests
        framework.suite
        tests/test_suite.c
    )

    umicom_add_framework_test(
        umicom-integration-application-tests
        framework.integration.application
        tests/test_integration_application.c
    )
    umicom_add_framework_test(
        umicom-integration-registry-tests
        framework.integration.registry
        tests/test_integration_registry.c
    )
    umicom_add_framework_test(
        umicom-integration-resolver-tests
        framework.integration.resolver
        tests/test_integration_resolver.c
    )
    umicom_add_framework_test(
        umicom-integration-presence-tests
        framework.integration.presence
        tests/test_integration_presence.c
    )
    umicom_add_framework_test(
        umicom-integration-router-tests
        framework.integration.router
        tests/test_integration_router.c
    )
    umicom_add_framework_test(
        umicom-integration-dispatcher-tests
        framework.integration.dispatcher
        tests/test_integration_dispatcher.c
    )
    umicom_add_framework_test(
        umicom-integration-launch-plan-tests
        framework.integration.launch_plan
        tests/test_integration_launch_plan.c
    )
    umicom_add_framework_test(
        umicom-integration-required-missing-tests
        framework.integration.required_missing
        tests/test_integration_required_missing.c
    )
    umicom_add_framework_test(
        umicom-integration-suite-runtime-tests
        framework.integration.suite_runtime
        tests/test_integration_suite_runtime.c
    )
    umicom_add_framework_test(
        umicom-integration-contract-tests
        framework.integration.contract
        tests/test_integration_contract.c
    )
    umicom_add_framework_test(
        umicom-integration-session-tests
        framework.integration.session
        tests/test_integration_session.c
    )
    umicom_add_framework_test(
        umicom-integration-topology-tests
        framework.integration.topology
        tests/test_integration_topology.c
    )
    umicom_add_framework_test(
        umicom-integration-health-tests
        framework.integration.health
        tests/test_integration_health.c
    )
    umicom_add_framework_test(
        umicom-integration-catalogue-tests
        framework.integration.catalogue
        tests/test_integration_catalogue.c
    )
    umicom_add_framework_test(
        umicom-integration-designer-types-tests
        framework.integration.designer_types
        tests/test_integration_designer_types.c
    )
    umicom_add_framework_test(
        umicom-integration-connection-profile-tests
        framework.integration.connection_profile
        tests/test_integration_connection_profile.c
    )
    umicom_add_framework_test(
        umicom-integration-api-contract-tests
        framework.integration.api_contract
        tests/test_integration_api_contract.c
    )
    umicom_add_framework_test(
        umicom-integration-schema-catalogue-tests
        framework.integration.schema_catalogue
        tests/test_integration_schema_catalogue.c
    )
    umicom_add_framework_test(
        umicom-integration-field-mapping-tests
        framework.integration.field_mapping
        tests/test_integration_field_mapping.c
    )
    umicom_add_framework_test(
        umicom-integration-workflow-tests
        framework.integration.workflow
        tests/test_integration_workflow.c
    )
    umicom_add_framework_test(
        umicom-integration-request-plan-tests
        framework.integration.request_plan
        tests/test_integration_request_plan.c
    )
    umicom_add_framework_test(
        umicom-integration-simulator-tests
        framework.integration.simulator
        tests/test_integration_simulator.c
    )
    umicom_add_framework_test(
        umicom-integration-designer-fabric-tests
        framework.integration.designer_fabric
        tests/test_integration_designer_fabric.c
    )
    umicom_add_framework_test(
        umicom-policy-tests
        framework.policy
        tests/test_policy.c
    )
    umicom_add_framework_test(
        umicom-process-tests
        framework.process
        tests/test_process.c
    )
    umicom_add_framework_test(
        umicom-toolchain-tests
        framework.toolchain
        tests/test_toolchain.c
    )
    # Batch 35 Toolchain & Environment Centre regression coverage.
    umicom_add_framework_test(
        umicom-toolchain-capability-tests
        framework.toolchain.capability
        tests/test_toolchain_capability.c
    )
    umicom_add_framework_test(
        umicom-toolchain-catalogue-tests
        framework.toolchain.catalogue
        tests/test_toolchain_catalogue.c
    )
    umicom_add_framework_test(
        umicom-toolchain-environment-composition-tests
        framework.toolchain.environment_composition
        tests/test_toolchain_environment_composition.c
    )
    umicom_add_framework_test(
        umicom-toolchain-compilation-database-tests
        framework.toolchain.compilation_database
        tests/test_toolchain_compilation_database.c
    )
    umicom_add_framework_test(
        umicom-toolchain-sdk-tests
        framework.toolchain.sdk
        tests/test_toolchain_sdk.c
    )
    umicom_add_framework_test(
        umicom-repository-tests
        framework.repository
        tests/test_repository.c
    )
    umicom_add_framework_test(
        umicom-repository-scaffold-tests
        framework.repository_scaffold
        tests/test_repository_scaffold.c
    )
    target_compile_definitions(umicom-repository-scaffold-tests PRIVATE
        UMICOM_REPOSITORY_TEMPLATE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/templates/repository"
    )

    umicom_add_framework_test(
        umicom-threading-tests
        framework.threading
        tests/test_threading.c
    )
    umicom_add_framework_test(
        umicom-task-queue-tests
        framework.task_queue
        tests/test_task_queue.c
    )
    umicom_add_framework_test(
        umicom-service-registry-tests
        framework.service_registry
        tests/test_service_registry.c
    )
    umicom_add_framework_test(
        umicom-command-registry-tests
        framework.command_registry
        tests/test_command_registry.c
    )
    umicom_add_framework_test(
        umicom-health-tests
        framework.health
        tests/test_health.c
    )
    umicom_add_framework_test(
        umicom-document-store-tests
        framework.document_store
        tests/test_document_store.c
    )
    umicom_add_framework_test(
        umicom-session-recovery-tests
        framework.session_recovery
        tests/test_session_recovery.c
    )
    umicom_add_framework_test(
        umicom-path-tests
        framework.path
        tests/test_path.c
    )
    umicom_add_framework_test(
        umicom-directory-tests
        framework.directory
        tests/test_directory.c
    )
    umicom_add_framework_test(
        umicom-cancellation-tests
        framework.cancellation
        tests/test_cancellation.c
    )
    umicom_add_framework_test(
        umicom-watcher-tests
        framework.watcher
        tests/test_watcher.c
    )
    umicom_add_framework_test(
        umicom-process-supervisor-tests
        framework.process_supervisor
        tests/test_process_supervisor.c
    )
    umicom_add_framework_test(
        umicom-workspace-graph-tests
        framework.workspace_graph
        tests/test_workspace_graph.c
    )
    umicom_add_framework_test(
        umicom-file-index-tests
        framework.file_index
        tests/test_file_index.c
    )
    umicom_add_framework_test(
        umicom-search-tests
        framework.search
        tests/test_search.c
    )

    umicom_add_framework_test(
        umicom-message-model-tests
        framework.message_model
        tests/test_message_model.c
    )
    umicom_add_framework_test(
        umicom-payload-tests
        framework.payload
        tests/test_payload.c
    )
    umicom_add_framework_test(
        umicom-schema-registry-tests
        framework.schema_registry
        tests/test_schema_registry.c
    )
    umicom_add_framework_test(
        umicom-channel-tests
        framework.channel
        tests/test_channel.c
    )
    umicom_add_framework_test(
        umicom-dispatcher-tests
        framework.dispatcher
        tests/test_dispatcher.c
    )
    umicom_add_framework_test(
        umicom-reliability-tests
        framework.reliability
        tests/test_reliability.c
    )
    umicom_add_framework_test(
        umicom-inbox-tests
        framework.inbox
        tests/test_inbox.c
    )
    umicom_add_framework_test(
        umicom-outbox-tests
        framework.outbox
        tests/test_outbox.c
    )
    umicom_add_framework_test(
        umicom-dead-letter-tests
        framework.dead_letter
        tests/test_dead_letter.c
    )
    umicom_add_framework_test(
        umicom-idempotency-tests
        framework.idempotency
        tests/test_idempotency.c
    )
    umicom_add_framework_test(
        umicom-delivery-lease-tests
        framework.delivery_lease
        tests/test_delivery_lease.c
    )
    umicom_add_framework_test(
        umicom-router-filter-transformer-tests
        framework.router_filter_transformer
        tests/test_router_filter_transformer.c
    )
    umicom_add_framework_test(
        umicom-aggregator-tests
        framework.aggregator
        tests/test_aggregator.c
    )
    umicom_add_framework_test(
        umicom-resequencer-tests
        framework.resequencer
        tests/test_resequencer.c
    )
    umicom_add_framework_test(
        umicom-transport-topic-metrics-tests
        framework.transport_topic_metrics
        tests/test_transport_topic_metrics.c
    )
    umicom_add_framework_test(
        umicom-replay-tests
        framework.replay
        tests/test_replay.c
    )
    umicom_add_framework_test(
        umicom-flow-workflow-saga-tests
        framework.flow_workflow_saga
        tests/test_flow_workflow_saga.c
    )
    umicom_add_framework_test(
        umicom-data-transaction-tests
        framework.data_transactions
        tests/test_data_transactions.c
    )
    umicom_add_framework_test(
        umicom-migration-tests
        framework.migrations
        tests/test_migrations.c
    )
    umicom_add_framework_test(
        umicom-store-repository-tests
        framework.store_repository
        tests/test_store_repository.c
    )
    umicom_add_framework_test(
        umicom-row-statement-tests
        framework.row_statement
        tests/test_row_statement.c
    )
    umicom_add_framework_test(
        umicom-blob-cache-tests
        framework.blob_cache
        tests/test_blob_cache.c
    )
    umicom_add_framework_test(
        umicom-metadata-lineage-tests
        framework.metadata_lineage
        tests/test_metadata_lineage.c
    )
    umicom_add_framework_test(
        umicom-message-store-tests
        framework.message_store
        tests/test_message_store.c
    )
    umicom_add_framework_test(
        umicom-integrity-tests
        framework.integrity
        tests/test_integrity.c
    )
    umicom_add_framework_test(
        umicom-sqlite-adapter-tests
        framework.sqlite_adapter
        tests/test_sqlite_adapter.c
    )
    umicom_add_framework_test(umicom-data-server-visit-snapshot-tests framework.data_server_visit_snapshot tests/test_data_server_visit_snapshot.c)
    umicom_add_framework_test(umicom-database-connections-tests framework.database_connections tests/test_database_connections.c)
    umicom_add_framework_test(umicom-database-schema-catalog-tests framework.database_schema_catalog tests/test_database_schema_catalog.c)
    umicom_add_framework_test(umicom-database-query-policy-tests framework.database_query_policy tests/test_database_query_policy.c)
    umicom_add_framework_test(umicom-database-query-session-tests framework.database_query_session tests/test_database_query_session.c)
    umicom_add_framework_test(umicom-database-explorer-transaction-tests framework.database_explorer_transaction tests/test_database_explorer_transaction.c)
    umicom_add_framework_test(umicom-database-migration-catalog-tests framework.database_migration_catalog tests/test_database_migration_catalog.c)
    umicom_add_framework_test(umicom-database-transfer-backup-tests framework.database_transfer_backup tests/test_database_transfer_backup.c)
    umicom_add_framework_test(umicom-database-explorer-tests framework.database_explorer tests/test_database_explorer.c)

    umicom_add_framework_test(umicom-ui-value-property-tests framework.ui_value_property tests/test_ui_value_property.c)
    umicom_add_framework_test(umicom-ui-view-model-tests framework.ui_view_model tests/test_ui_view_model.c)
    umicom_add_framework_test(umicom-ui-action-menu-tests framework.ui_action_menu tests/test_ui_action_menu.c)
    umicom_add_framework_test(umicom-ui-toolbar-status-tests framework.ui_toolbar_status tests/test_ui_toolbar_status.c)
    umicom_add_framework_test(umicom-ui-notification-dialog-tests framework.ui_notification_dialog tests/test_ui_notification_dialog.c)
    umicom_add_framework_test(umicom-ui-theme-tests framework.ui_theme tests/test_ui_theme.c)
    umicom_add_framework_test(umicom-ui-selection-clipboard-tests framework.ui_selection_clipboard tests/test_ui_selection_clipboard.c)
    umicom_add_framework_test(umicom-ui-accessibility-tests framework.ui_accessibility tests/test_ui_accessibility.c)
    umicom_add_framework_test(umicom-ui-pane-document-tests framework.ui_pane_document tests/test_ui_pane_document.c)
    umicom_add_framework_test(umicom-ui-editor-perspective-tests framework.ui_editor_perspective tests/test_ui_editor_perspective.c)
    umicom_add_framework_test(umicom-ui-layout-tests framework.ui_layout tests/test_ui_layout.c)
    umicom_add_framework_test(umicom-ui-contribution-tests framework.ui_contribution tests/test_ui_contribution.c)
    umicom_add_framework_test(umicom-ui-binding-tests framework.ui_binding tests/test_ui_binding.c)
    umicom_add_framework_test(umicom-ui-event-tests framework.ui_event tests/test_ui_event.c)
    umicom_add_framework_test(umicom-ui-adapter-tests framework.ui_adapter tests/test_ui_adapter.c)
    umicom_add_framework_test(umicom-ui-view-factory-tests framework.ui_view_factory tests/test_ui_view_factory.c)
    umicom_add_framework_test(umicom-ui-workbench-tests framework.ui_workbench tests/test_ui_workbench.c)
    umicom_add_framework_test(umicom-ui-application-shell-tests framework.ui_application_shell tests/test_ui_application_shell.c)
    umicom_add_framework_test(umicom-ui-headless-tests framework.ui_headless tests/test_ui_headless.c)
    umicom_add_framework_test(umicom-ui-command-view-tests framework.ui.command_view tests/test_ui_command_view.c)

    if(UMICOM_BUILD_NATIVE_TOOL)
        add_test(NAME framework.native_cli.version COMMAND umicom version)
        add_test(NAME framework.native_cli.capabilities COMMAND umicom capabilities)
        add_test(
            NAME framework.native_cli.repository_dry_run
            COMMAND umicom create repo "Umicom Designer"
                    --frontends console
                    --framework-installed
                    --no-git
                    --dry-run
        )
    endif()


    # Build, testing, terminal, protocol and version-control contracts.
    umicom_add_framework_test(umicom-build-profile-tests framework.build_profile tests/test_build_profile.c)
    umicom_add_framework_test(umicom-build-diagnostic-tests framework.build_diagnostics tests/test_build_diagnostics.c)
    umicom_add_framework_test(umicom-build-parser-tests framework.build_parser tests/test_build_parser.c)
    umicom_add_framework_test(umicom-build-history-tests framework.build_history tests/test_build_history.c)
    umicom_add_framework_test(umicom-build-graph-tests framework.build_graph tests/test_build_graph.c)
    umicom_add_framework_test(umicom-build-policy-tests framework.build_policy tests/test_build_policy.c)
    umicom_add_framework_test(umicom-build-plan-tests framework.build_plan tests/test_build_plan.c)
    umicom_add_framework_test(umicom-build-provider-command-tests framework.build_provider_commands tests/test_build_provider_commands.c)
    umicom_add_framework_test(umicom-build-engine-tests framework.build_engine tests/test_build_engine.c)
    umicom_add_framework_test(umicom-build-artifact-tests framework.build_artifact tests/test_build_artifact.c)
    umicom_add_framework_test(umicom-build-workspace-tests framework.build_workspace tests/test_build_workspace.c)
    umicom_add_framework_test(umicom-build-ui-views-tests framework.build_ui_views tests/test_build_ui_views.c)
    umicom_add_framework_test(umicom-testing-registry-tests framework.testing_registry tests/test_testing_registry.c)
    umicom_add_framework_test(umicom-testing-discovery-tests framework.testing_discovery tests/test_testing_discovery.c)
    umicom_add_framework_test(umicom-terminal-command-tests framework.terminal_command tests/test_terminal_command.c)
    umicom_add_framework_test(umicom-terminal-transcript-tests framework.terminal_transcript tests/test_terminal_transcript.c)
    umicom_add_framework_test(umicom-terminal-environment-tests framework.terminal_environment tests/test_terminal_environment.c)
    umicom_add_framework_test(umicom-terminal-profile-tests framework.terminal_profile tests/test_terminal_profile.c)
    umicom_add_framework_test(umicom-terminal-history-tests framework.terminal_history tests/test_terminal_history.c)
    umicom_add_framework_test(umicom-terminal-scan-tests framework.terminal_scan tests/test_terminal_scan.c)
    umicom_add_framework_test(umicom-terminal-search-tests framework.terminal_search tests/test_terminal_search.c)
    umicom_add_framework_test(umicom-terminal-link-tests framework.terminal_link tests/test_terminal_link.c)
    umicom_add_framework_test(umicom-terminal-ansi-tests framework.terminal_ansi tests/test_terminal_ansi.c)
    umicom_add_framework_test(umicom-terminal-tab-model-tests framework.terminal_tab_model tests/test_terminal_tab_model.c)
    umicom_add_framework_test(umicom-terminal-split-model-tests framework.terminal_split_model tests/test_terminal_split_model.c)
    umicom_add_framework_test(umicom-terminal-event-tests framework.terminal_event tests/test_terminal_event.c)
    umicom_add_framework_test(umicom-terminal-controller-tests framework.terminal_controller tests/test_terminal_controller.c)
    umicom_add_framework_test(umicom-terminal-ui-views-tests framework.terminal_ui_views tests/test_terminal_ui_views.c)
    umicom_add_framework_test(umicom-protocol-framing-tests framework.protocol_framing tests/test_protocol_framing.c)
    umicom_add_framework_test(umicom-protocol-json-tests framework.protocol_json tests/test_protocol_json.c)
    umicom_add_framework_test(umicom-json-rpc-tests framework.json_rpc tests/test_json_rpc.c)
    umicom_add_framework_test(umicom-lsp-protocol-tests framework.lsp_protocol tests/test_lsp_protocol.c)
    umicom_add_framework_test(umicom-dap-protocol-tests framework.dap_protocol tests/test_dap_protocol.c)
    umicom_add_framework_test(umicom-editor-platform-tests framework.editor_platform tests/test_editor_platform.c)
    umicom_add_framework_test(umicom-editor-presentation-tests framework.editor_presentation tests/test_editor_presentation.c)
    umicom_add_framework_test(umicom-ui-editor-theme-tests framework.ui.editor_theme tests/test_ui_editor_theme.c)
    umicom_add_framework_test(umicom-editor-feature-catalog-tests framework.editor_feature_catalog tests/editor/test_editor_feature_catalog.c)
    umicom_add_framework_test(umicom-editor-text-buffer-tests framework.editor_text_buffer tests/editor/test_editor_text_buffer.c)
    umicom_add_framework_test(umicom-editor-line-index-tests framework.editor_line_index tests/editor/test_editor_line_index.c)
    umicom_add_framework_test(umicom-editor-search-engine-tests framework.editor_search_engine tests/editor/test_editor_search_engine.c)
    umicom_add_framework_test(umicom-editor-edit-history-tests framework.editor_edit_history tests/editor/test_editor_edit_history.c)
    umicom_add_framework_test(umicom-editor-edit-command-tests framework.editor_edit_commands tests/editor/test_editor_edit_commands.c)
    umicom_add_framework_test(umicom-editor-edit-engine-tests framework.editor_edit_engine tests/editor/test_editor_edit_engine.c)
    umicom_add_framework_test(umicom-editor-source-navigation-tests framework.editor_source_navigation tests/editor/test_editor_source_navigation.c)
    umicom_add_framework_test(umicom-editor-navigation-result-tests framework.editor_navigation_results tests/editor/test_editor_navigation_results.c)
    umicom_add_framework_test(umicom-editor-source-bookmark-tests framework.editor_source_bookmarks tests/editor/test_editor_source_bookmarks.c)
    umicom_add_framework_test(umicom-editor-symbol-index-tests framework.editor_symbol_index tests/editor/test_editor_symbol_index.c)
    umicom_add_framework_test(umicom-editor-symbol-relationship-tests framework.editor_symbol_relationships tests/editor/test_editor_symbol_relationships.c)
    umicom_add_framework_test(umicom-editor-document-outline-tests framework.editor_document_outline tests/editor/test_editor_document_outline.c)
    umicom_add_framework_test(umicom-editor-breadcrumb-model-tests framework.editor_breadcrumb_model tests/editor/test_editor_breadcrumb_model.c)
    umicom_add_framework_test(umicom-editor-source-navigation-service-tests framework.editor_source_navigation_service tests/editor/test_editor_source_navigation_service.c)
    umicom_add_framework_test(umicom-editor-navigation-command-tests framework.editor_navigation_commands tests/editor/test_editor_navigation_commands.c)
    umicom_add_framework_test(umicom-editor-peek-definition-tests framework.editor_peek_definition tests/editor/test_editor_peek_definition.c)
    umicom_add_framework_test(umicom-editor-reference-results-tests framework.editor_reference_results tests/editor/test_editor_reference_results.c)
    umicom_add_framework_test(umicom-editor-symbol-hierarchy-tests framework.editor_symbol_hierarchy tests/editor/test_editor_symbol_hierarchy.c)
    umicom_add_framework_test(umicom-editor-call-hierarchy-tests framework.editor_call_hierarchy tests/editor/test_editor_call_hierarchy.c)
    umicom_add_framework_test(umicom-editor-type-hierarchy-tests framework.editor_type_hierarchy tests/editor/test_editor_type_hierarchy.c)
    umicom_add_framework_test(umicom-editor-navigation-insights-tests framework.editor_navigation_insights tests/editor/test_editor_navigation_insights.c)
    umicom_add_framework_test(umicom-editor-workspace-edit-tests framework.editor_workspace_edit tests/editor/test_editor_workspace_edit.c)
    umicom_add_framework_test(umicom-editor-rename-symbol-tests framework.editor_rename_symbol tests/editor/test_editor_rename_symbol.c)
    umicom_add_framework_test(umicom-editor-code-lens-model-tests framework.editor_code_lens_model tests/editor/test_editor_code_lens_model.c)
    umicom_add_framework_test(umicom-editor-inlay-hint-model-tests framework.editor_inlay_hint_model tests/editor/test_editor_inlay_hint_model.c)
    umicom_add_framework_test(umicom-editor-semantic-highlighting-tests framework.editor_semantic_highlighting tests/editor/test_editor_semantic_highlighting.c)
    umicom_add_framework_test(umicom-editor-intelligence-tests framework.editor_intelligence tests/editor/test_editor_intelligence.c)
    umicom_add_framework_test(umicom-editor-intelligence-command-tests framework.editor_intelligence_commands tests/editor/test_editor_intelligence_commands.c)
    umicom_add_framework_test(umicom-editor-hover-model-tests framework.editor_hover_model tests/editor/test_editor_hover_model.c)
    umicom_add_framework_test(umicom-editor-signature-help-model-tests framework.editor_signature_help_model tests/editor/test_editor_signature_help_model.c)
    umicom_add_framework_test(umicom-editor-parameter-information-tests framework.editor_parameter_information tests/editor/test_editor_parameter_information.c)
    umicom_add_framework_test(umicom-editor-quick-documentation-tests framework.editor_quick_documentation tests/editor/test_editor_quick_documentation.c)
    umicom_add_framework_test(umicom-editor-assistance-tests framework.editor_assistance tests/editor/test_editor_assistance.c)
    umicom_add_framework_test(umicom-editor-assistance-command-tests framework.editor_assistance_commands tests/editor/test_editor_assistance_commands.c)
    umicom_add_framework_test(umicom-editor-formatting-profile-tests framework.editor_formatting_profile tests/editor/test_editor_formatting_profile.c)
    umicom_add_framework_test(umicom-editor-formatting-session-tests framework.editor_formatting_session tests/editor/test_editor_formatting_session.c)
    umicom_add_framework_test(umicom-editor-import-organisation-tests framework.editor_import_organisation tests/editor/test_editor_import_organisation.c)
    umicom_add_framework_test(umicom-editor-snippet-session-tests framework.editor_snippet_session tests/editor/test_editor_snippet_session.c)
    umicom_add_framework_test(umicom-editor-linked-editing-model-tests framework.editor_linked_editing_model tests/editor/test_editor_linked_editing_model.c)
    umicom_add_framework_test(umicom-editor-editing-productivity-tests framework.editor_editing_productivity tests/editor/test_editor_editing_productivity.c)
    umicom_add_framework_test(umicom-editor-productivity-command-tests framework.editor_productivity_commands tests/editor/test_editor_productivity_commands.c)
    umicom_add_framework_test(umicom-editor-code-action-provider-tests framework.editor_code_action_provider tests/editor/test_editor_code_action_provider.c)
    umicom_add_framework_test(umicom-editor-code-action-query-tests framework.editor_code_action_query tests/editor/test_editor_code_action_query.c)
    umicom_add_framework_test(umicom-editor-refactoring-plan-tests framework.editor_refactoring_plan tests/editor/test_editor_refactoring_plan.c)
    umicom_add_framework_test(umicom-editor-refactoring-preview-tests framework.editor_refactoring_preview tests/editor/test_editor_refactoring_preview.c)
    umicom_add_framework_test(umicom-editor-edit-transaction-tests framework.editor_edit_transaction tests/editor/test_editor_edit_transaction.c)
    umicom_add_framework_test(umicom-editor-code-action-orchestration-tests framework.editor_code_action_orchestration tests/editor/test_editor_code_action_orchestration.c)
    umicom_add_framework_test(umicom-editor-refactoring-command-tests framework.editor_refactoring_commands tests/editor/test_editor_refactoring_command.c)
    umicom_add_framework_test(umicom-editor-text-scan-tests framework.editor_text_scan tests/editor/test_editor_text_scan.c)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomEditorFeatureTests.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomDocumentCapabilityTests.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/UmicomDeveloperCapabilityTests.cmake")
    umicom_add_developer_capability_tests()
    umicom_add_framework_test(umicom-developer-capability-catalogue-tests framework.developer.capability_catalogue tests/test_developer_capability_catalogue.c)
    umicom_add_framework_test(umicom-document-capability-catalogue-tests framework.document.capability_catalogue tests/document/test_document_capability_catalogue.c)
    umicom_add_framework_test(umicom-document-encoding-tests framework.document.encoding tests/document/test_document_encoding.c)
    umicom_add_framework_test(umicom-document-line-endings-tests framework.document.line_endings tests/document/test_document_line_endings.c)
    umicom_add_framework_test(umicom-document-uri-tests framework.document.uri tests/document/test_document_uri.c)
    umicom_add_framework_test(umicom-document-fingerprint-tests framework.document.fingerprint tests/document/test_document_fingerprint.c)
    umicom_add_framework_test(umicom-document-provider-tests framework.document.provider tests/document/test_document_provider.c)
    umicom_add_framework_test(umicom-document-loader-saver-tests framework.document.loader_saver tests/document/test_document_loader_saver.c)
    umicom_add_framework_test(umicom-document-language-filter-tests framework.document.language_filter tests/document/test_document_language_filter.c)
    umicom_add_framework_test(umicom-document-coordinator-tests framework.document.coordinator tests/document/test_document_coordinator.c)
    umicom_add_framework_test(umicom-document-commands-tests framework.document.commands tests/document/test_document_commands.c)
    umicom_add_framework_test(umicom-document-conflict-tests framework.document.conflict tests/document/test_document_conflict.c)
    umicom_add_framework_test(umicom-vcs-status-tests framework.vcs_status tests/test_vcs_status.c)
    umicom_add_framework_test(umicom-vcs-history-tests framework.vcs_history tests/test_vcs_history.c)
    umicom_add_framework_test(umicom-vcs-diff-tests framework.vcs_diff tests/test_vcs_diff.c)
    umicom_add_framework_test(umicom-vcs-provider-tests framework.vcs_provider tests/test_vcs_provider.c)
    umicom_add_framework_test(umicom-vcs-collections-tests framework.vcs_collections tests/test_vcs_collections.c)
    umicom_add_framework_test(umicom-vcs-diff-document-tests framework.vcs_diff_document tests/test_vcs_diff_document.c)
    umicom_add_framework_test(umicom-vcs-operation-log-tests framework.vcs_operation_log tests/test_vcs_operation_log.c)
    umicom_add_framework_test(umicom-vcs-repository-tests framework.vcs_repository tests/test_vcs_repository.c)
    umicom_add_framework_test(umicom-vcs-workspace-tests framework.vcs_workspace tests/test_vcs_workspace.c)
    umicom_add_framework_test(umicom-vcs-workspace-coordinator-tests framework.vcs_workspace_coordinator tests/test_vcs_workspace_coordinator.c)
    umicom_add_framework_test(umicom-vcs-ui-views-tests framework.vcs_ui_views tests/test_vcs_ui_views.c)
    umicom_add_framework_test(umicom-vcs-ui-workspace-views-tests framework.vcs_ui_workspace_views tests/test_vcs_ui_workspace_views.c)
    umicom_add_framework_test(umicom-vcs-git-cli-tests framework.vcs_git_cli tests/test_vcs_git_cli.c)

    umicom_add_framework_test(umicom-declarative-value-tests framework.declarative_value tests/test_declarative_value.c)
    umicom_add_framework_test(umicom-declarative-node-tests framework.declarative_node tests/test_declarative_node.c)
    umicom_add_framework_test(umicom-declarative-document-tests framework.declarative_document tests/test_declarative_document.c)
    umicom_add_framework_test(umicom-declarative-parser-tests framework.declarative_parser tests/test_declarative_parser.c)
    umicom_add_framework_test(umicom-declarative-serializer-tests framework.declarative_serializer tests/test_declarative_serializer.c)
    umicom_add_framework_test(umicom-declarative-catalogue-tests framework.declarative_catalogue tests/test_declarative_catalogue.c)
    umicom_add_framework_test(umicom-declarative-validator-tests framework.declarative_validator tests/test_declarative_validator.c)
    umicom_add_framework_test(umicom-declarative-compiler-tests framework.declarative_compiler tests/test_declarative_compiler.c)
    umicom_add_framework_test(umicom-declarative-template-tests framework.declarative_templates tests/test_declarative_templates.c)
    umicom_add_framework_test(umicom-declarative-renderer-tests framework.declarative_renderer tests/test_declarative_renderer.c)
    umicom_add_framework_test(umicom-designer-history-tests framework.designer_history tests/test_designer_history.c)
    umicom_add_framework_test(umicom-designer-selection-tests framework.designer_selection tests/test_designer_selection.c)
    umicom_add_framework_test(umicom-designer-preview-tests framework.designer_preview tests/test_designer_preview.c)
    umicom_add_framework_test(umicom-designer-project-tests framework.designer_project tests/test_designer_project.c)
    umicom_add_framework_test(umicom-designer-types-tree-tests framework.designer.types_tree tests/test_designer_types_tree.c)
    umicom_add_framework_test(umicom-designer-property-tests framework.designer.property tests/test_designer_property.c)
    umicom_add_framework_test(umicom-designer-binding-tests framework.designer.binding tests/test_designer_binding.c)
    umicom_add_framework_test(umicom-designer-interaction-tests framework.designer.interaction tests/test_designer_interaction.c)
    umicom_add_framework_test(umicom-designer-drag-drop-tests framework.designer.drag_drop tests/test_designer_drag_drop.c)
    umicom_add_framework_test(umicom-designer-transaction-tests framework.designer.transaction tests/test_designer_transaction.c)
    umicom_add_framework_test(umicom-designer-clipboard-tests framework.designer.clipboard tests/test_designer_clipboard.c)
    umicom_add_framework_test(umicom-designer-template-tests framework.designer.template tests/test_designer_template.c)
    umicom_add_framework_test(umicom-designer-layout-tests framework.designer.layout tests/test_designer_layout.c)
    umicom_add_framework_test(umicom-designer-session-tests framework.designer.session tests/test_designer_session.c)


    # Web Server, browser, charting and multi-frontend contracts.
    umicom_add_framework_test(umicom-web-types-tests framework.web_types tests/test_web_types.c)
    umicom_add_framework_test(umicom-web-request-tests framework.web_request tests/test_web_request.c)
    umicom_add_framework_test(umicom-web-response-tests framework.web_response tests/test_web_response.c)
    umicom_add_framework_test(umicom-web-url-query-tests framework.web_url_query tests/test_web_url_query.c)
    umicom_add_framework_test(umicom-web-parser-tests framework.web_parser tests/test_web_parser.c)
    umicom_add_framework_test(umicom-web-router-tests framework.web_router tests/test_web_router.c)
    umicom_add_framework_test(umicom-web-middleware-tests framework.web_middleware tests/test_web_middleware.c)
    umicom_add_framework_test(umicom-web-json-streams-tests framework.web_json_streams tests/test_web_json_streams.c)
    umicom_add_framework_test(umicom-websocket-tests framework.websocket tests/test_websocket.c)
    umicom_add_framework_test(umicom-web-sessions-tests framework.web_sessions tests/test_web_sessions.c)
    umicom_add_framework_test(umicom-web-endpoint-metrics-tests framework.web_endpoint_metrics tests/test_web_endpoint_metrics.c)
    umicom_add_framework_test(umicom-web-runtime-tests framework.web_runtime tests/test_web_runtime.c)
    umicom_add_framework_test(umicom-web-server-config-tests framework.web_server_config tests/test_web_server_config.c)
    umicom_add_framework_test(umicom-browser-tests framework.browser tests/test_browser.c)
    umicom_add_framework_test(umicom-browser-policy-tests framework.browser_policy tests/test_browser_policy.c)
    umicom_add_framework_test(umicom-chart-model-tests framework.chart_model tests/test_chart_model.c)
    umicom_add_framework_test(umicom-chart-indicator-tests framework.chart_indicator tests/test_chart_indicator.c)
    umicom_add_framework_test(umicom-frontend-plan-tests framework.frontend_plan tests/test_frontend_plan.c)
    umicom_add_framework_test(umicom-frontend-headless-tests framework.frontend_headless tests/test_frontend_headless.c)
    umicom_add_framework_test(umicom-frontend-devices-tests framework.frontend_devices tests/test_frontend_devices.c)
    umicom_add_framework_test(umicom-frontend-documents-reload-tests framework.frontend_documents_reload tests/test_frontend_documents_reload.c)
    umicom_add_framework_test(umicom-frontend-dom-inspector-tests framework.frontend_dom_inspector tests/test_frontend_dom_inspector.c)
    umicom_add_framework_test(umicom-frontend-devtools-logs-tests framework.frontend_devtools_logs tests/test_frontend_devtools_logs.c)
    umicom_add_framework_test(umicom-frontend-build-profiles-tests framework.frontend_build_profiles tests/test_frontend_build_profiles.c)
    umicom_add_framework_test(umicom-frontend-quality-audit-tests framework.frontend_quality_audit tests/test_frontend_quality_audit.c)
    umicom_add_framework_test(umicom-frontend-dev-server-preview-tests framework.frontend_dev_server_preview tests/test_frontend_dev_server_preview.c)
    umicom_add_framework_test(umicom-frontend-studio-tests framework.frontend_studio tests/test_frontend_studio.c)

    umicom_add_framework_test(umicom-security-identity-tests framework.security_identity tests/test_security_identity.c)
    umicom_add_framework_test(umicom-security-roles-tests framework.security_roles tests/test_security_roles.c)
    umicom_add_framework_test(umicom-security-session-tests framework.security_session tests/test_security_session.c)
    umicom_add_framework_test(umicom-security-authorisation-tests framework.security_authorisation tests/test_security_authorisation.c)
    umicom_add_framework_test(umicom-security-context-tests framework.security_context tests/test_security_context.c)
    umicom_add_framework_test(umicom-security-redaction-tests framework.security_redaction tests/test_security_redaction.c)
    umicom_add_framework_test(umicom-security-identity-governance-tests framework.security_identity_governance tests/test_security_identity_governance.c)
    umicom_add_framework_test(umicom-security-permission-policy-tests framework.security_permission_policy tests/test_security_permission_policy.c)
    umicom_add_framework_test(umicom-security-workspace-policy-tests framework.security_workspace_policy tests/test_security_workspace_policy.c)
    umicom_add_framework_test(umicom-security-secret-inventory-tests framework.security_secret_inventory tests/test_security_secret_inventory.c)
    umicom_add_framework_test(umicom-security-redaction-policy-tests framework.security_redaction_policy tests/test_security_redaction_policy.c)
    umicom_add_framework_test(umicom-security-approval-tests framework.security_approval tests/test_security_approval.c)
    umicom_add_framework_test(umicom-security-package-trust-tests framework.security_package_trust tests/test_security_package_trust.c)
    umicom_add_framework_test(umicom-security-governance-audit-tests framework.security_governance_audit tests/test_security_governance_audit.c)
    umicom_add_framework_test(umicom-security-centre-tests framework.security_centre tests/test_security_centre.c)
    umicom_add_framework_test(umicom-metrics-tests framework.metrics tests/test_metrics.c)
    umicom_add_framework_test(umicom-trace-tests framework.trace tests/test_trace.c)
    umicom_add_framework_test(umicom-audit-tests framework.audit tests/test_audit.c)
    umicom_add_framework_test(umicom-profiler-tests framework.profiler tests/test_profiler.c)
    umicom_add_framework_test(umicom-readiness-tests framework.readiness tests/test_readiness.c)
    umicom_add_framework_test(umicom-retry-tests framework.retry tests/test_retry.c)
    umicom_add_framework_test(umicom-circuit-breaker-tests framework.circuit_breaker tests/test_circuit_breaker.c)
    umicom_add_framework_test(umicom-rate-limiter-tests framework.rate_limiter tests/test_rate_limiter.c)
    umicom_add_framework_test(umicom-supervisor-tests framework.resilience_supervisor tests/test_supervisor.c)
    umicom_add_framework_test(umicom-health-gate-tests framework.health_gate tests/test_health_gate.c)
    umicom_add_framework_test(umicom-plugin-manifest-tests framework.plugin_manifest tests/test_plugin_manifest.c)
    umicom_add_framework_test(umicom-plugin-registry-tests framework.plugin_registry tests/test_plugin_registry.c)
    umicom_add_framework_test(umicom-plugin-contributions-tests framework.plugin_contributions tests/test_plugin_contributions.c)
    umicom_add_framework_test(umicom-plugin-host-tests framework.plugin_host tests/test_plugin_host.c)
    umicom_add_framework_test(umicom-plugin-checksum-tests framework.plugin_checksum tests/test_plugin_checksum.c)
    umicom_add_framework_test(umicom-plugin-dependency-tests framework.plugin_dependency tests/test_plugin_dependency.c)
    umicom_add_framework_test(umicom-plugin-extension-point-tests framework.plugin_extension_point tests/test_plugin_extension_point.c)
    umicom_add_framework_test(umicom-plugin-policy-tests framework.plugin_policy tests/test_plugin_policy.c)
    umicom_add_framework_test(umicom-plugin-package-tests framework.plugin_package tests/test_plugin_package.c)
    umicom_add_framework_test(umicom-plugin-event-log-tests framework.plugin_event_log tests/test_plugin_event_log.c)
    umicom_add_framework_test(umicom-plugin-catalogue-tests framework.plugin_catalogue tests/test_plugin_catalogue.c)
    umicom_add_framework_test(umicom-plugin-sdk-tests framework.plugin_sdk tests/test_plugin_sdk.c)
    umicom_add_framework_test(umicom-plugin-manager-tests framework.plugin_manager tests/test_plugin_manager.c)
    umicom_add_framework_test(umicom-plugin-ui-tests framework.plugin_ui tests/test_plugin_ui.c)

    umicom_add_framework_test(umicom-ai-model-tests framework.ai_model tests/test_ai_model.c)
    umicom_add_framework_test(umicom-ai-prompt-context-tests framework.ai_prompt_context tests/test_ai_prompt_context.c)
    umicom_add_framework_test(umicom-ai-retrieval-tests framework.ai_retrieval tests/test_ai_retrieval.c)
    umicom_add_framework_test(umicom-ai-embedding-tests framework.ai_embedding tests/test_ai_embedding.c)
    umicom_add_framework_test(umicom-ai-tools-policy-tests framework.ai_tools_policy tests/test_ai_tools_policy.c)
    umicom_add_framework_test(umicom-ai-provider-tests framework.ai_provider tests/test_ai_provider.c)
    umicom_add_framework_test(umicom-ai-authorengine-tests framework.ai_authorengine tests/test_ai_authorengine.c)
    # AuthorEngine Integration v2 keeps orchestration outside Studio while
    # testing each reusable boundary as a focused, diagnosable contract.
    umicom_add_framework_test(umicom-ai-authorengine-boundary-tests framework.ai_authorengine_boundary tests/test_ai_authorengine_boundary.c)
    umicom_add_framework_test(umicom-ai-runtime-catalogue-tests framework.ai_runtime_catalogue tests/test_ai_runtime_catalogue.c)
    umicom_add_framework_test(umicom-ai-context-broker-tests framework.ai_context_broker tests/test_ai_context_broker.c)
    umicom_add_framework_test(umicom-ai-privacy-policy-tests framework.ai_privacy_policy tests/test_ai_privacy_policy.c)
    umicom_add_framework_test(umicom-ai-conversation-archive-tests framework.ai_conversation_archive tests/test_ai_conversation_archive.c)
    umicom_add_framework_test(umicom-ai-authorengine-service-tests framework.ai_authorengine_service tests/test_ai_authorengine_service.c)
    umicom_add_framework_test(umicom-ai-ui-tests framework.ai_ui tests/test_ai_ui.c)
    # Batch 48: reusable coding assistance, deterministic repository context
    # and conflict-aware patch transactions behind AuthorEngine governance.
    umicom_add_framework_test(umicom-ai-coding-types-tests framework.ai_coding_types tests/test_ai_coding_types.c)
    umicom_add_framework_test(umicom-ai-coding-context-tests framework.ai_coding_context tests/test_ai_coding_context.c)
    umicom_add_framework_test(umicom-ai-coding-patch-tests framework.ai_coding_patch tests/test_ai_coding_patch.c)
    umicom_add_framework_test(umicom-ai-coding-assistant-tests framework.ai_coding_assistant tests/test_ai_coding_assistant.c)
    umicom_add_framework_test(umicom-ai-coding-ui-tests framework.ai_coding_ui tests/test_ai_coding_ui.c)
    # Batch 49: Native RAG, provenance, incremental refresh and offline archive.
    umicom_add_framework_test(umicom-knowledge-types-tests framework.knowledge_types tests/test_knowledge_types.c)
    umicom_add_framework_test(umicom-knowledge-chunker-tests framework.knowledge_chunker tests/test_knowledge_chunker.c)
    umicom_add_framework_test(umicom-knowledge-provider-tests framework.knowledge_provider tests/test_knowledge_provider.c)
    umicom_add_framework_test(umicom-knowledge-vector-index-tests framework.knowledge_vector_index tests/test_knowledge_vector_index.c)
    umicom_add_framework_test(umicom-knowledge-service-tests framework.knowledge_service tests/test_knowledge_service.c)
    umicom_add_framework_test(umicom-knowledge-archive-tests framework.knowledge_archive tests/test_knowledge_archive.c)
    umicom_add_framework_test(umicom-knowledge-ui-tests framework.knowledge_ui tests/test_knowledge_ui.c)
    umicom_add_framework_test(umicom-helix-fitness-tests framework.helix_fitness tests/test_helix_fitness.c)
    umicom_add_framework_test(umicom-helix-runtime-tests framework.helix_runtime tests/test_helix_runtime.c)
    umicom_add_framework_test(umicom-helix-release-gate-tests framework.helix_release_gate tests/test_helix_release_gate.c)
    umicom_add_framework_test(umicom-helix-memory-tests framework.helix_memory tests/test_helix_memory.c)
    # Batch 50: governed specialist agents, operation state, evidence,
    # approval, bounded retry, candidate comparison and rollback.
    umicom_add_framework_test(umicom-helix-types-tests framework.helix_types tests/test_helix_types.c)
    umicom_add_framework_test(umicom-helix-agents-tests framework.helix_agents tests/test_helix_agents.c)
    umicom_add_framework_test(umicom-helix-approval-tests framework.helix_approval tests/test_helix_approval.c)
    umicom_add_framework_test(umicom-helix-journal-tests framework.helix_journal tests/test_helix_journal.c)
    umicom_add_framework_test(umicom-helix-retry-tests framework.helix_retry tests/test_helix_retry.c)
    umicom_add_framework_test(umicom-helix-candidates-tests framework.helix_candidates tests/test_helix_candidates.c)
    umicom_add_framework_test(umicom-helix-rollback-tests framework.helix_rollback tests/test_helix_rollback.c)
    umicom_add_framework_test(umicom-helix-provenance-tests framework.helix_provenance tests/test_helix_provenance.c)
    umicom_add_framework_test(umicom-helix-orchestrator-tests framework.helix_orchestrator tests/test_helix_orchestrator.c)
    umicom_add_framework_test(umicom-helix-ui-tests framework.helix_ui tests/test_helix_ui.c)

    # Application delivery, packaging, update and release contracts.
    umicom_add_framework_test(umicom-delivery-delivery-types-test framework.delivery_delivery_types tests/test_delivery_types.c)
    umicom_add_framework_test(umicom-delivery-delivery-artifacts-test framework.delivery_delivery_artifacts tests/test_delivery_artifacts.c)
    umicom_add_framework_test(umicom-delivery-delivery-manifest-evidence-test framework.delivery_delivery_manifest_evidence tests/test_delivery_manifest_evidence.c)
    umicom_add_framework_test(umicom-delivery-delivery-stage-package-test framework.delivery_delivery_stage_package tests/test_delivery_stage_package.c)
    umicom_add_framework_test(umicom-delivery-delivery-checksum-integrity-test framework.delivery_delivery_checksum_integrity tests/test_delivery_checksum_integrity.c)
    umicom_add_framework_test(umicom-delivery-delivery-signature-signing-test framework.delivery_delivery_signature_signing tests/test_delivery_signature_signing.c)
    umicom_add_framework_test(umicom-delivery-delivery-sbom-license-test framework.delivery_delivery_sbom_license tests/test_delivery_sbom_license.c)
    umicom_add_framework_test(umicom-delivery-delivery-provenance-release-test framework.delivery_delivery_provenance_release tests/test_delivery_provenance_release.c)
    umicom_add_framework_test(umicom-delivery-delivery-release-registry-test framework.delivery_delivery_release_registry tests/test_delivery_release_registry.c)
    umicom_add_framework_test(umicom-delivery-delivery-update-rollback-test framework.delivery_delivery_update_rollback tests/test_delivery_update_rollback.c)
    umicom_add_framework_test(umicom-delivery-delivery-install-test framework.delivery_delivery_install tests/test_delivery_install.c)
    umicom_add_framework_test(umicom-delivery-delivery-bundle-dependencies-test framework.delivery_delivery_bundle_dependencies tests/test_delivery_bundle_dependencies.c)
    umicom_add_framework_test(umicom-delivery-delivery-smoke-gate-test framework.delivery_delivery_smoke_gate tests/test_delivery_smoke_gate.c)
    umicom_add_framework_test(umicom-delivery-delivery-pipeline-test framework.delivery_delivery_pipeline tests/test_delivery_pipeline.c)
    umicom_add_framework_test(umicom-delivery-delivery-policy-report-test framework.delivery_delivery_policy_report tests/test_delivery_policy_report.c)
    umicom_add_framework_test(umicom-delivery-delivery-inventory-environment-test framework.delivery_delivery_inventory_environment tests/test_delivery_inventory_environment.c)
    umicom_add_framework_test(umicom-delivery-delivery-deployment-health-test framework.delivery_delivery_deployment_health tests/test_delivery_deployment_health.c)
    umicom_add_framework_test(umicom-delivery-delivery-metadata-store-test framework.delivery_delivery_metadata_store tests/test_delivery_metadata_store.c)
    umicom_add_framework_test(umicom-delivery-delivery-audit-channels-test framework.delivery_delivery_audit_channels tests/test_delivery_audit_channels.c)
    umicom_add_framework_test(umicom-delivery-delivery-verification-candidate-test framework.delivery_delivery_verification_candidate tests/test_delivery_verification_candidate.c)
    umicom_add_framework_test(umicom-delivery-delivery-release-controls-test framework.delivery_delivery_release_controls tests/test_delivery_release_controls.c)
    umicom_add_framework_test(umicom-delivery-delivery-release-notes-platforms-test framework.delivery_delivery_release_notes_platforms tests/test_delivery_release_notes_platforms.c)
    umicom_add_framework_test(umicom-delivery-delivery-artifact-index-test framework.delivery_delivery_artifact_index tests/test_delivery_artifact_index.c)

    # ABI, SDK and independent-product release hardening tests.
    umicom_add_framework_test(umicom-abi-version-test framework.abi_version tests/test_abi_version.c)
    umicom_add_framework_test(umicom-abi-hash-test framework.abi_hash tests/test_abi_hash.c)
    umicom_add_framework_test(umicom-abi-platform-test framework.abi_platform tests/test_abi_platform.c)
    umicom_add_framework_test(umicom-abi-descriptor-test framework.abi_descriptor tests/test_abi_descriptor.c)
    umicom_add_framework_test(umicom-abi-compatibility-test framework.abi_compatibility tests/test_abi_compatibility.c)
    umicom_add_framework_test(umicom-abi-catalogue-test framework.abi_catalogue tests/test_abi_catalogue.c)
    umicom_add_framework_test(umicom-abi-baseline-test framework.abi_baseline tests/test_abi_baseline.c)
    umicom_add_framework_test(umicom-sdk-manifest-test framework.sdk_manifest tests/test_sdk_manifest.c)
    umicom_add_framework_test(umicom-sdk-package-test framework.sdk_package tests/test_sdk_package.c)
    umicom_add_framework_test(umicom-sdk-compatibility-test framework.sdk_compatibility tests/test_sdk_compatibility.c)
    umicom_add_framework_test(umicom-sdk-conformance-test framework.sdk_conformance tests/test_sdk_conformance.c)
    umicom_add_framework_test(umicom-sdk-export-catalogue-test framework.sdk_export_catalogue tests/test_sdk_export_catalogue.c)
    umicom_add_framework_test(umicom-sdk-consumer-package-test framework.sdk_consumer_package tests/test_sdk_consumer_package.c)
    umicom_add_framework_test(umicom-delivery-package-manifest-test framework.delivery_package_manifest tests/test_delivery_package_manifest.c)
    umicom_add_framework_test(umicom-delivery-runtime-bundle-test framework.delivery_runtime_bundle tests/test_delivery_runtime_bundle.c)
    umicom_add_framework_test(umicom-delivery-installer-test framework.delivery_installer tests/test_delivery_installer.c)
    umicom_add_framework_test(umicom-delivery-release-evidence-test framework.delivery_release_evidence tests/test_delivery_release_evidence.c)
    umicom_add_framework_test(umicom-delivery-update-upgrade-test framework.delivery_update_upgrade tests/test_delivery_update_upgrade.c)
    umicom_add_framework_test(umicom-delivery-product-release-test framework.delivery_product_release tests/test_delivery_product_release.c)
    umicom_add_framework_test(umicom-product-reference-test framework.product_reference tests/test_product_reference.c)
    umicom_add_framework_test(umicom-product-registry-test framework.product_registry tests/test_product_registry.c)
    umicom_add_framework_test(umicom-product-suite-test framework.product_suite tests/test_product_suite.c)
    umicom_add_framework_test(umicom-product-manifest-test framework.product_manifest tests/test_product_manifest.c)
    umicom_add_framework_test(umicom-product-bootstrap-test framework.product_bootstrap tests/test_product_bootstrap.c)

    # Product, Marketplace and Update Centre distribution contracts.
    umicom_add_framework_test(umicom-distribution-package-test framework.distribution_package tests/test_distribution_package.c)
    umicom_add_framework_test(umicom-distribution-repository-test framework.distribution_repository tests/test_distribution_repository.c)
    umicom_add_framework_test(umicom-distribution-evidence-policy-test framework.distribution_evidence_policy tests/test_distribution_evidence_policy.c)
    umicom_add_framework_test(umicom-distribution-transaction-test framework.distribution_transaction tests/test_distribution_transaction.c)
    umicom_add_framework_test(umicom-distribution-notification-test framework.distribution_notification tests/test_distribution_notification.c)
    umicom_add_framework_test(umicom-distribution-service-test framework.distribution_service tests/test_distribution_service.c)
    umicom_add_framework_test(umicom-distribution-ui-test framework.distribution_ui tests/test_distribution_ui.c)

    # Independent reference consumers compile only against public Framework contracts.
    foreach(UMICOM_REFERENCE_PRODUCT designer trader tms media_studio studio)
        add_executable(umicom-reference-${UMICOM_REFERENCE_PRODUCT}
            examples/reference_products/${UMICOM_REFERENCE_PRODUCT}/main.c
            examples/reference_products/${UMICOM_REFERENCE_PRODUCT}/reference_product.c
        )
        target_include_directories(umicom-reference-${UMICOM_REFERENCE_PRODUCT} PRIVATE
            examples/reference_products/${UMICOM_REFERENCE_PRODUCT}
        )
        target_link_libraries(umicom-reference-${UMICOM_REFERENCE_PRODUCT} PRIVATE Umicom::Framework)
        umicom_apply_warnings(umicom-reference-${UMICOM_REFERENCE_PRODUCT})
        umicom_apply_sanitizers(umicom-reference-${UMICOM_REFERENCE_PRODUCT})

        add_executable(umicom-reference-${UMICOM_REFERENCE_PRODUCT}-test
            examples/reference_products/${UMICOM_REFERENCE_PRODUCT}/test_reference_product.c
            examples/reference_products/${UMICOM_REFERENCE_PRODUCT}/reference_product.c
        )
        target_include_directories(umicom-reference-${UMICOM_REFERENCE_PRODUCT}-test PRIVATE
            examples/reference_products/${UMICOM_REFERENCE_PRODUCT}
        )
        target_link_libraries(umicom-reference-${UMICOM_REFERENCE_PRODUCT}-test PRIVATE Umicom::Framework)
        umicom_apply_warnings(umicom-reference-${UMICOM_REFERENCE_PRODUCT}-test)
        umicom_apply_sanitizers(umicom-reference-${UMICOM_REFERENCE_PRODUCT}-test)
        add_test(NAME framework.reference_${UMICOM_REFERENCE_PRODUCT}
                 COMMAND umicom-reference-${UMICOM_REFERENCE_PRODUCT}-test)
    endforeach()
endif()


# CodeGuard focused security and architecture tests.
if(BUILD_TESTING)
    umicom_add_framework_test(umicom-codeguard-result-tests framework.codeguard_result tests/test_codeguard_result.c)
    umicom_add_framework_test(umicom-codeguard-lifetime-tests framework.codeguard_lifetime tests/test_codeguard_lifetime.c)
    umicom_add_framework_test(umicom-codeguard-duplicate-tests framework.codeguard_duplicate tests/test_codeguard_duplicate.c)
    umicom_add_framework_test(umicom-codeguard-report-tests framework.codeguard_report tests/test_codeguard_report.c)
    umicom_add_framework_test(umicom-codeguard-quality-gate-tests framework.codeguard_quality_gate tests/test_codeguard_quality_gate.c)
    umicom_add_framework_test(umicom-codeguard-external-tool-tests framework.codeguard_external_tool tests/test_codeguard_external_tool.c)
    umicom_add_framework_test(umicom-codeguard-evidence-tests framework.codeguard_evidence tests/test_codeguard_evidence.c)
    umicom_add_framework_test(umicom-codeguard-architecture-policy-tests framework.codeguard_architecture_policy tests/test_codeguard_architecture_policy.c)
    umicom_add_framework_test(umicom-codeguard-unsafe-api-tests framework.codeguard_unsafe_api tests/test_codeguard_unsafe_api.c)
    umicom_add_framework_test(umicom-codeguard-memory-portability-tests framework.codeguard_memory_portability tests/test_codeguard_memory_portability.c)
    umicom_add_framework_test(umicom-codeguard-duplicate-evidence-tests framework.codeguard_duplicate_evidence tests/test_codeguard_duplicate_evidence.c)
    umicom_add_framework_test(umicom-codeguard-api-baseline-tests framework.codeguard_api_baseline tests/test_codeguard_api_baseline.c)
    umicom_add_framework_test(umicom-codeguard-quality-evaluation-tests framework.codeguard_quality_evaluation tests/test_codeguard_quality_evaluation.c)
    umicom_add_framework_test(umicom-codeguard-remediation-tests framework.codeguard_remediation tests/test_codeguard_remediation.c)
    umicom_add_framework_test(umicom-codeguard-quality-session-tests framework.codeguard_quality_session tests/test_codeguard_quality_session.c)
    umicom_add_framework_test(umicom-codeguard-rules-allocation-tests framework.codeguard_rules_allocation tests/test_codeguard_rules_allocation.c)
    umicom_add_framework_test(umicom-codeguard-rules-arithmetic-tests framework.codeguard_rules_arithmetic tests/test_codeguard_rules_arithmetic.c)
    umicom_add_framework_test(umicom-codeguard-rules-atomic-tests framework.codeguard_rules_atomic tests/test_codeguard_rules_atomic.c)
    umicom_add_framework_test(umicom-codeguard-rules-buffer-tests framework.codeguard_rules_buffer tests/test_codeguard_rules_buffer.c)
    umicom_add_framework_test(umicom-codeguard-rules-command-tests framework.codeguard_rules_command tests/test_codeguard_rules_command.c)
    umicom_add_framework_test(umicom-codeguard-rules-concurrency-tests framework.codeguard_rules_concurrency tests/test_codeguard_rules_concurrency.c)
    umicom_add_framework_test(umicom-codeguard-rules-coupling_rules-tests framework.codeguard_rules_coupling_rules tests/test_codeguard_rules_coupling_rules.c)
    umicom_add_framework_test(umicom-codeguard-rules-database_boundary-tests framework.codeguard_rules_database_boundary tests/test_codeguard_rules_database_boundary.c)
    umicom_add_framework_test(umicom-codeguard-rules-duplicate_rules-tests framework.codeguard_rules_duplicate_rules tests/test_codeguard_rules_duplicate_rules.c)
    umicom_add_framework_test(umicom-codeguard-rules-environment-tests framework.codeguard_rules_environment tests/test_codeguard_rules_environment.c)
    umicom_add_framework_test(umicom-codeguard-rules-error-tests framework.codeguard_rules_error tests/test_codeguard_rules_error.c)
    umicom_add_framework_test(umicom-codeguard-rules-file-tests framework.codeguard_rules_file tests/test_codeguard_rules_file.c)
    umicom_add_framework_test(umicom-codeguard-rules-format-tests framework.codeguard_rules_format tests/test_codeguard_rules_format.c)
    umicom_add_framework_test(umicom-codeguard-rules-global_state-tests framework.codeguard_rules_global_state tests/test_codeguard_rules_global_state.c)
    umicom_add_framework_test(umicom-codeguard-rules-input-tests framework.codeguard_rules_input tests/test_codeguard_rules_input.c)
    umicom_add_framework_test(umicom-codeguard-rules-integer-tests framework.codeguard_rules_integer tests/test_codeguard_rules_integer.c)
    umicom_add_framework_test(umicom-codeguard-rules-lifetime_rules-tests framework.codeguard_rules_lifetime_rules tests/test_codeguard_rules_lifetime_rules.c)
    umicom_add_framework_test(umicom-codeguard-rules-lock-tests framework.codeguard_rules_lock tests/test_codeguard_rules_lock.c)
    umicom_add_framework_test(umicom-codeguard-rules-logging-tests framework.codeguard_rules_logging tests/test_codeguard_rules_logging.c)
    umicom_add_framework_test(umicom-codeguard-rules-memory-tests framework.codeguard_rules_memory tests/test_codeguard_rules_memory.c)
    umicom_add_framework_test(umicom-codeguard-rules-network-tests framework.codeguard_rules_network tests/test_codeguard_rules_network.c)
    umicom_add_framework_test(umicom-codeguard-rules-path-tests framework.codeguard_rules_path tests/test_codeguard_rules_path.c)
    umicom_add_framework_test(umicom-codeguard-rules-pointer-tests framework.codeguard_rules_pointer tests/test_codeguard_rules_pointer.c)
    umicom_add_framework_test(umicom-codeguard-rules-portability-tests framework.codeguard_rules_portability tests/test_codeguard_rules_portability.c)
    umicom_add_framework_test(umicom-codeguard-rules-private_header-tests framework.codeguard_rules_private_header tests/test_codeguard_rules_private_header.c)
    umicom_add_framework_test(umicom-codeguard-rules-process-tests framework.codeguard_rules_process tests/test_codeguard_rules_process.c)
    umicom_add_framework_test(umicom-codeguard-rules-resource-tests framework.codeguard_rules_resource tests/test_codeguard_rules_resource.c)
    umicom_add_framework_test(umicom-codeguard-rules-secret-tests framework.codeguard_rules_secret tests/test_codeguard_rules_secret.c)
    umicom_add_framework_test(umicom-codeguard-rules-sql-tests framework.codeguard_rules_sql tests/test_codeguard_rules_sql.c)
    umicom_add_framework_test(umicom-codeguard-rules-string-tests framework.codeguard_rules_string tests/test_codeguard_rules_string.c)
    umicom_add_framework_test(umicom-codeguard-rules-temp_file-tests framework.codeguard_rules_temp_file tests/test_codeguard_rules_temp_file.c)
    umicom_add_framework_test(umicom-codeguard-rules-ui_boundary-tests framework.codeguard_rules_ui_boundary tests/test_codeguard_rules_ui_boundary.c)
endif()

# Trading and financial foundation tests.
if(BUILD_TESTING)
    umicom_add_framework_test(umicom-trading-cash-buying-power-test framework.trading_cash_buying_power tests/trading/test_cash_buying_power.c)
    umicom_add_framework_test(umicom-trading-environment-health-test framework.trading_environment_health tests/trading/test_environment_health.c)
    umicom_add_framework_test(umicom-trading-execution-capture-test framework.trading_execution_capture tests/trading/test_execution_capture.c)
    umicom_add_framework_test(umicom-trading-finance-foundation-test framework.trading_finance_foundation tests/trading/test_finance_foundation.c)
    umicom_add_framework_test(umicom-trading-fix-venue-contracts-test framework.trading_fix_venue_contracts tests/trading/test_fix_venue_contracts.c)
    umicom_add_framework_test(umicom-trading-ibkr-boundary-test framework.trading_ibkr_boundary tests/trading/test_ibkr_boundary.c)
    umicom_add_framework_test(umicom-trading-kill-switch-throttle-test framework.trading_kill_switch_throttle tests/trading/test_kill_switch_throttle.c)
    umicom_add_framework_test(umicom-trading-market-data-core-test framework.trading_market_data_core tests/trading/test_market_data_core.c)
    umicom_add_framework_test(umicom-trading-market-depth-book-test framework.trading_market_depth_book tests/trading/test_market_depth_book.c)
    umicom_add_framework_test(umicom-trading-market-features-test framework.trading_market_features tests/trading/test_market_features.c)
    umicom_add_framework_test(umicom-trading-market-session-gap-test framework.trading_market_session_gap tests/trading/test_market_session_gap.c)
    umicom_add_framework_test(umicom-trading-oms-test framework.trading_oms tests/trading/test_oms.c)
    umicom_add_framework_test(umicom-trading-order-lifecycle-test framework.trading_order_lifecycle tests/trading/test_order_lifecycle.c)
    umicom_add_framework_test(umicom-trading-order-store-fill-test framework.trading_order_store_fill tests/trading/test_order_store_fill.c)
    umicom_add_framework_test(umicom-trading-position-book-portfolio-test framework.trading_position_book_portfolio tests/trading/test_position_book_portfolio.c)
    umicom_add_framework_test(umicom-trading-positions-pnl-test framework.trading_positions_pnl tests/trading/test_positions_pnl.c)
    umicom_add_framework_test(umicom-trading-replay-test framework.trading_replay tests/trading/test_replay.c)
    umicom_add_framework_test(umicom-trading-research-factors-test framework.trading_research_factors tests/trading/test_research_factors.c)
    umicom_add_framework_test(umicom-trading-research-movement-test framework.trading_research_movement tests/trading/test_research_movement.c)
    umicom_add_framework_test(umicom-trading-risk-engine-test framework.trading_risk_engine tests/trading/test_risk_engine.c)
    umicom_add_framework_test(umicom-trading-simulated-broker-test framework.trading_simulated_broker tests/trading/test_simulated_broker.c)
    umicom_add_framework_test(umicom-trading-surveillance-test framework.trading_surveillance tests/trading/test_surveillance.c)
    umicom_add_framework_test(umicom-trading-trading-profile-test framework.trading_trading_profile tests/trading/test_trading_profile.c)
    umicom_add_framework_test(umicom-trading-watchlist-alerts-test framework.trading_watchlist_alerts tests/trading/test_watchlist_alerts.c)
    umicom_add_framework_test(umicom-trading-workspace-test framework.trading_workspace tests/trading/test_trading_workspace.c)
    umicom_add_framework_test(umicom-trading-ui-views-test framework.trading_ui_views tests/trading/test_trading_ui_views.c)
    foreach(UMICOM_TRADING_TEST_TARGET
        umicom-trading-cash-buying-power-test
        umicom-trading-environment-health-test
        umicom-trading-execution-capture-test
        umicom-trading-finance-foundation-test
        umicom-trading-fix-venue-contracts-test
        umicom-trading-ibkr-boundary-test
        umicom-trading-kill-switch-throttle-test
        umicom-trading-market-data-core-test
        umicom-trading-market-depth-book-test
        umicom-trading-market-features-test
        umicom-trading-market-session-gap-test
        umicom-trading-oms-test
        umicom-trading-order-lifecycle-test
        umicom-trading-order-store-fill-test
        umicom-trading-position-book-portfolio-test
        umicom-trading-positions-pnl-test
        umicom-trading-replay-test
        umicom-trading-research-factors-test
        umicom-trading-research-movement-test
        umicom-trading-risk-engine-test
        umicom-trading-simulated-broker-test
        umicom-trading-surveillance-test
        umicom-trading-trading-profile-test
        umicom-trading-watchlist-alerts-test
        umicom-trading-workspace-test
        umicom-trading-ui-views-test
    )
        target_include_directories(${UMICOM_TRADING_TEST_TARGET} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/trading
        )
    endforeach()
endif()


# Reusable semantic UI component tests.
if(BUILD_TESTING)
    umicom_add_framework_test(umicom-ui-component-spec-test framework.ui_component_spec tests/ui_components/test_ui_component_spec.c)
    umicom_add_framework_test(umicom-ui-component-registry-test framework.ui_component_registry tests/ui_components/test_ui_component_registry.c)
    umicom_add_framework_test(umicom-ui-component-tree-test framework.ui_component_tree tests/ui_components/test_ui_component_tree.c)
    umicom_add_framework_test(umicom-ui-component-catalogue-test framework.ui_component_catalogue tests/ui_components/test_ui_component_catalogue.c)
    umicom_add_framework_test(umicom-ui-component-model-test framework.ui_component_model tests/ui_components/test_ui_component_model.c)
    umicom_add_framework_test(umicom-ui-component-validation-test framework.ui_component_validation tests/ui_components/test_ui_component_validation.c)
    umicom_add_framework_test(umicom-ui-form-test framework.ui_form tests/ui_components/test_ui_form.c)
    umicom_add_framework_test(umicom-ui-form-validation-test framework.ui_form_validation tests/ui_components/test_ui_form_validation.c)
    umicom_add_framework_test(umicom-ui-component-command-test framework.ui_component_command tests/ui_components/test_ui_component_command.c)
    umicom_add_framework_test(umicom-ui-component-set-test framework.ui_component_set tests/ui_components/test_ui_component_set.c)
    umicom_add_framework_test(umicom-ui-component-host-test framework.ui_component_host tests/ui_components/test_component_host.c)
    foreach(UMICOM_UI_COMPONENT_TEST_TARGET
        umicom-ui-component-spec-test
        umicom-ui-component-registry-test
        umicom-ui-component-tree-test
        umicom-ui-component-catalogue-test
        umicom-ui-component-model-test
        umicom-ui-component-validation-test
        umicom-ui-form-test
        umicom-ui-form-validation-test
        umicom-ui-component-command-test
        umicom-ui-component-set-test
        umicom-ui-component-host-test
    )
        target_link_libraries(${UMICOM_UI_COMPONENT_TEST_TARGET} PRIVATE Umicom::ui_components)
    endforeach()
endif()

# Framework-authoritative application portfolio, federation and GTK4 coverage.
if(BUILD_TESTING)
    # Application architecture tests link the bounded public application target
    # rather than the complete Framework aggregate. This keeps the dependency
    # graph honest and makes focused governance validation practical.
    function(umicom_add_application_test target test_name source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE Umicom::application)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        add_test(NAME ${test_name} COMMAND ${target})
        set_tests_properties(${test_name} PROPERTIES
            LABELS "framework;application")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    umicom_add_application_test(
        umicom-application-portfolio-test
        framework.application_portfolio
        tests/application/test_application_portfolio.c)
    umicom_add_application_test(
        umicom-application-boundary-audit-test
        framework.application_boundary_audit
        tests/application/test_application_boundary_audit.c)
    umicom_add_application_test(
        umicom-application-federation-test
        framework.application_federation
        tests/application/test_application_federation.c)
    umicom_add_application_test(
        umicom-application-context-hub-test
        framework.application_context_hub
        tests/application/test_application_context_hub.c)
    umicom_add_application_test(
        umicom-application-resource-broker-test
        framework.application_resource_broker
        tests/application/test_application_resource_broker.c)
    umicom_add_application_test(
        umicom-master-controller-application-authority-test
        framework.master_controller_application_authority
        tests/application/test_master_controller_application_authority.c)
    umicom_add_application_test(
        umicom-application-component-catalogue-test
        framework.application_component_catalogue
        tests/application/test_application_component_catalogue.c)
    umicom_add_application_test(
        umicom-application-component-contract-registry-test
        framework.application_component.contract_registry
        tests/application_component/test_contract_registry.c)
    umicom_add_application_test(
        umicom-application-component-factory-instance-test
        framework.application_component.factory_instance
        tests/application_component/test_factory_instance.c)
    umicom_add_application_test(
        umicom-application-component-selection-layout-test
        framework.application_component.selection_layout
        tests/application_component/test_selection_layout.c)
    umicom_add_application_test(
        umicom-application-component-frontend-adapter-test
        framework.application_component.frontend_adapter
        tests/application_component/test_frontend_adapter.c)
    set_tests_properties(
        framework.application_component.contract_registry
        framework.application_component.factory_instance
        framework.application_component.selection_layout
        framework.application_component.frontend_adapter
        PROPERTIES LABELS "framework;application;component;composition")

    umicom_add_application_test(
        umicom-component-governance-evidence-test
        framework.component_governance.evidence
        tests/application/test_component_governance_evidence.c)
    set_tests_properties(framework.component_governance.evidence PROPERTIES
        LABELS "framework;application;governance;mb60")
    umicom_add_application_test(
        umicom-component-governance-policy-test
        framework.component_governance.policy
        tests/application/test_component_governance_policy.c)
    set_tests_properties(framework.component_governance.policy PROPERTIES
        LABELS "framework;application;governance;mb60")
    umicom_add_application_test(
        umicom-component-governance-inventory-test
        framework.component_governance.inventory
        tests/application/test_component_governance_inventory.c)
    set_tests_properties(framework.component_governance.inventory PROPERTIES
        LABELS "framework;application;governance;mb60")
    umicom_add_application_test(
        umicom-component-governance-query-test
        framework.component_governance.query
        tests/application/test_component_governance_query.c)
    set_tests_properties(framework.component_governance.query PROPERTIES
        LABELS "framework;application;governance;mb60")
    umicom_add_application_test(
        umicom-component-governance-release-gate-test
        framework.component_governance.release_gate
        tests/application/test_component_governance_release_gate.c)
    set_tests_properties(framework.component_governance.release_gate PROPERTIES
        LABELS "framework;application;governance;mb60")
    umicom_add_application_test(
        umicom-component-governance-report-test
        framework.component_governance.report
        tests/application/test_component_governance_report.c)
    set_tests_properties(framework.component_governance.report PROPERTIES
        LABELS "framework;application;governance;mb60")
    umicom_add_framework_test(
        umicom-gtk4-widget-catalogue-test
        framework.gtk4_widget_catalogue
        tests/application/test_gtk4_widget_catalogue.c)
    umicom_add_framework_test(
        umicom-application-ui-views-test
        framework.application_ui_views
        tests/application/test_application_ui_views.c)
    umicom_add_framework_test(
        umicom-desktop-monitor-topology-test
        framework.desktop_monitor_topology
        tests/desktop/test_monitor_topology.c)
    umicom_add_framework_test(
        umicom-desktop-layout-tabs-test
        framework.desktop_layout_tabs
        tests/desktop/test_layout_tabs.c)
    umicom_add_framework_test(
        umicom-desktop-window-manager-test
        framework.desktop_window_manager
        tests/desktop/test_window_manager.c)
    umicom_add_framework_test(
        umicom-desktop-layout-catalogue-test
        framework.desktop_layout_catalogue
        tests/desktop/test_layout_catalogue.c)
    umicom_add_framework_test(
        umicom-desktop-context-links-test
        framework.desktop_context_links
        tests/desktop/test_context_links.c)
    umicom_add_framework_test(
        umicom-desktop-persistence-test
        framework.desktop_persistence
        tests/desktop/test_persistence.c)
    umicom_add_framework_test(
        umicom-desktop-runtime-state-test
        framework.desktop_runtime_state
        tests/desktop/test_runtime_state.c)
    umicom_add_framework_test(
        umicom-desktop-layout-history-test
        framework.desktop_layout_history
        tests/desktop/test_layout_history.c)
    umicom_add_framework_test(
        umicom-desktop-session-recovery-test
        framework.desktop_session_recovery
        tests/desktop/test_session_recovery.c)
    umicom_add_framework_test(
        umicom-desktop-monitor-interaction-test
        framework.desktop_monitor_interaction
        tests/desktop/test_monitor_interaction.c)
    umicom_add_framework_test(
        umicom-master-controller-desktop-authority-test
        framework.master_controller_desktop_authority
        tests/desktop/test_master_controller_desktop_authority.c)
    umicom_add_framework_test(
        umicom-desktop-content-runtime-test
        framework.desktop_content_runtime
        tests/desktop/test_content_runtime.c)
    umicom_add_framework_test(
        umicom-desktop-component-drag-drop-test
        framework.desktop_component_drag_drop
        tests/desktop/test_component_drag_drop.c)
    umicom_add_framework_test(
        umicom-desktop-context-synchronizer-test
        framework.desktop_context_synchronizer
        tests/desktop/test_context_synchronizer.c)
    umicom_add_framework_test(
        umicom-desktop-snap-engine-test
        framework.desktop_snap_engine
        tests/desktop/test_snap_engine.c)
    umicom_add_framework_test(
        umicom-desktop-monitor-canvas-test
        framework.desktop_monitor_canvas
        tests/desktop/test_monitor_canvas.c)
    umicom_add_framework_test(
        umicom-desktop-layout-designer-test
        framework.desktop_layout_designer
        tests/desktop/test_layout_designer.c)
    umicom_add_framework_test(
        umicom-desktop-shell-model-test
        framework.desktop_shell_model
        tests/desktop/test_shell_model.c)
    umicom_add_framework_test(
        umicom-desktop-views-test
        framework.desktop_views
        tests/desktop/test_desktop_views.c)
endif()

# -----------------------------------------------------------------------------
# Installation and CMake package export
# -----------------------------------------------------------------------------
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY templates/ DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/templates)
install(DIRECTORY examples/sdk_consumer/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/sdk_consumer)

install(TARGETS
    umicom_base
    umicom_diagnostics
    umicom_messaging
    umicom_data
    umicom_platform
    umicom_security
    umicom_runtime
    umicom_integration
    umicom_resilience
    umicom_observability
    umicom_toolchain
    umicom_compiler
    umicom_repository
    umicom_plugin
    umicom_plugin_ui
    umicom_scaffold
    umicom_ui
    umicom_ui_components
    umicom_ui_headless
    umicom_build
    umicom_build_ui
    umicom_diagnostic_ui
    umicom_testing
    umicom_terminal
    umicom_terminal_ui
    umicom_protocol
    umicom_editor
    umicom_document
    umicom_vcs
    umicom_vcs_ui
    umicom_declarative
    umicom_designer
    umicom_web
    umicom_browser
    umicom_chart
    umicom_frontend
    umicom_ai
    umicom_ai_ui
    umicom_helix
    umicom_delivery
    umicom_abi
    umicom_sdk
    umicom_product
    umicom_application
    umicom_application_ui
    umicom_desktop
    umicom_distribution
    umicom_distribution_ui
    umicom_finance
    umicom_trading
    umicom_trading_ui
    umicom_codeguard
    umicom_project
    umicom_language
    umicom_debug
    umicom_source_control
    umicom_test_platform
    umicom_test_ui
    umicom_developer
    umicom_framework
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(UMICOM_BUILD_GTK4_ADAPTER)
    install(TARGETS umicom_ui_gtk4
        EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(UMICOM_BUILD_NATIVE_TOOL)
    install(TARGETS umicom RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

install(TARGETS umicom-codeguard RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT UmicomFrameworkTargets
    FILE UmicomFrameworkTargets.cmake
    NAMESPACE Umicom::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework
)

set(UMICOM_FRAMEWORK_FIND_SQLITE "")
if(SQLite3_FOUND)
    set(UMICOM_FRAMEWORK_FIND_SQLITE "find_dependency(SQLite3)")
endif()

configure_package_config_file(
    cmake/UmicomFrameworkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework
)

message(STATUS "Umicom Framework version: ${PROJECT_VERSION}")
message(STATUS "C standard: C${CMAKE_C_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "SQLite adapter available: ${SQLite3_FOUND}")
message(STATUS "Native umicom command: ${UMICOM_BUILD_NATIVE_TOOL}")
