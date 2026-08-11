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
    VERSION 0.5.0
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
)
add_library(Umicom::platform ALIAS umicom_platform)
set_target_properties(umicom_platform PROPERTIES EXPORT_NAME platform)
target_link_libraries(umicom_platform PUBLIC
    Umicom::base
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
    src/ui/editor.c
    src/ui/perspective.c
    src/ui/layout.c
    src/ui/contribution.c
    src/ui/binding.c
    src/ui/event.c
    src/ui/adapter.c
    src/ui/view_factory.c
    src/ui/workbench.c
    src/ui/application_shell.c
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

add_library(umicom_ui_headless STATIC
    adapters/headless/headless_adapter.c
    adapters/headless/shell_headless.c
    adapters/headless/workbench_headless.c
    adapters/headless/panes_headless.c
    adapters/headless/documents_headless.c
    adapters/headless/status_headless.c
    adapters/headless/notifications_headless.c
    adapters/headless/dialogs_headless.c
    adapters/headless/selection_headless.c
)
add_library(Umicom::ui_headless ALIAS umicom_ui_headless)
set_target_properties(umicom_ui_headless PROPERTIES EXPORT_NAME ui_headless)
target_link_libraries(umicom_ui_headless PUBLIC Umicom::ui)
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
    pkg_check_modules(UMICOM_FRAMEWORK_GTK4 REQUIRED IMPORTED_TARGET gtk4)
    add_library(umicom_ui_gtk4 STATIC
        adapters/gtk4/gtk4_adapter.c
        adapters/gtk4/shell_gtk4.c
        adapters/gtk4/workbench_gtk4.c
        adapters/gtk4/pane_gtk4.c
        adapters/gtk4/editor_gtk4.c
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
    )
    add_library(Umicom::ui_gtk4 ALIAS umicom_ui_gtk4)
    set_target_properties(umicom_ui_gtk4 PROPERTIES EXPORT_NAME ui_gtk4)
    target_link_libraries(umicom_ui_gtk4 PUBLIC
        Umicom::ui
        PkgConfig::UMICOM_FRAMEWORK_GTK4
    )
    target_include_directories(umicom_ui_gtk4
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/adapters/gtk4
    )
    umicom_apply_warnings(umicom_ui_gtk4)
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
add_library(umicom_terminal STATIC
    src/terminal/types.c
    src/terminal/command.c
    src/terminal/transcript.c
    src/terminal/environment.c
    src/terminal/session.c
    src/terminal/manager.c
    src/terminal/shell.c
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
umicom_apply_warnings(umicom_terminal)
umicom_apply_sanitizers(umicom_terminal)

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
# Provider-neutral version control and Git CLI adapter
# -----------------------------------------------------------------------------
add_library(umicom_vcs STATIC
    src/vcs/types.c
    src/vcs/change.c
    src/vcs/status.c
    src/vcs/branch.c
    src/vcs/commit.c
    src/vcs/history.c
    src/vcs/diff.c
    src/vcs/provider.c
    src/vcs/repository.c
    src/vcs/git_cli.c
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
umicom_apply_sanitizers(umicom_plugin)

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
    src/frontend/page.c
    src/frontend/route.c
    src/frontend/asset.c
    src/frontend/plan.c
    src/frontend/renderer.c
    src/frontend/generator.c
    src/frontend/gtkjs.c
    src/frontend/headless.c
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
    src/ai/chunk.c
    src/ai/chunker.c
    src/ai/citation.c
    src/ai/context.c
    src/ai/conversation.c
    src/ai/dataset.c
    src/ai/document.c
    src/ai/embedding.c
    src/ai/evaluation.c
    src/ai/health.c
    src/ai/local_runtime.c
    src/ai/message.c
    src/ai/model.c
    src/ai/policy.c
    src/ai/prompt.c
    src/ai/provider.c
    src/ai/provider_registry.c
    src/ai/rag.c
    src/ai/remote_provider.c
    src/ai/request.c
    src/ai/response.c
    src/ai/retrieval.c
    src/ai/runtime.c
    src/ai/session.c
    src/ai/source.c
    src/ai/stream.c
    src/ai/token_budget.c
    src/ai/tool.c
    src/ai/tool_registry.c
    src/ai/types.c
    src/ai/vector_store.c
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
)
add_library(Umicom::helix ALIAS umicom_helix)
set_target_properties(umicom_helix PROPERTIES EXPORT_NAME helix)
target_link_libraries(umicom_helix PUBLIC
    Umicom::base
    Umicom::ai
    Umicom::runtime
    Umicom::security
)
target_include_directories(umicom_helix PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_helix)
umicom_apply_sanitizers(umicom_helix)

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
    Umicom::resilience
    Umicom::toolchain
    Umicom::repository
    Umicom::plugin
    Umicom::scaffold
    Umicom::ui
    Umicom::ui_headless
    Umicom::build
    Umicom::testing
    Umicom::terminal
    Umicom::protocol
    Umicom::vcs
    Umicom::declarative
    Umicom::designer
    Umicom::web
    Umicom::browser
    Umicom::chart
    Umicom::frontend
    Umicom::ai
    Umicom::helix
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

if(BUILD_TESTING)
    function(umicom_add_framework_test target test_name source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE Umicom::Framework)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        add_test(NAME ${test_name} COMMAND ${target})
    endfunction()

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
    umicom_add_framework_test(umicom-testing-registry-tests framework.testing_registry tests/test_testing_registry.c)
    umicom_add_framework_test(umicom-testing-discovery-tests framework.testing_discovery tests/test_testing_discovery.c)
    umicom_add_framework_test(umicom-terminal-command-tests framework.terminal_command tests/test_terminal_command.c)
    umicom_add_framework_test(umicom-terminal-transcript-tests framework.terminal_transcript tests/test_terminal_transcript.c)
    umicom_add_framework_test(umicom-terminal-environment-tests framework.terminal_environment tests/test_terminal_environment.c)
    umicom_add_framework_test(umicom-protocol-framing-tests framework.protocol_framing tests/test_protocol_framing.c)
    umicom_add_framework_test(umicom-protocol-json-tests framework.protocol_json tests/test_protocol_json.c)
    umicom_add_framework_test(umicom-json-rpc-tests framework.json_rpc tests/test_json_rpc.c)
    umicom_add_framework_test(umicom-lsp-protocol-tests framework.lsp_protocol tests/test_lsp_protocol.c)
    umicom_add_framework_test(umicom-dap-protocol-tests framework.dap_protocol tests/test_dap_protocol.c)
    umicom_add_framework_test(umicom-vcs-status-tests framework.vcs_status tests/test_vcs_status.c)
    umicom_add_framework_test(umicom-vcs-history-tests framework.vcs_history tests/test_vcs_history.c)
    umicom_add_framework_test(umicom-vcs-diff-tests framework.vcs_diff tests/test_vcs_diff.c)
    umicom_add_framework_test(umicom-vcs-provider-tests framework.vcs_provider tests/test_vcs_provider.c)

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

    umicom_add_framework_test(umicom-security-identity-tests framework.security_identity tests/test_security_identity.c)
    umicom_add_framework_test(umicom-security-roles-tests framework.security_roles tests/test_security_roles.c)
    umicom_add_framework_test(umicom-security-session-tests framework.security_session tests/test_security_session.c)
    umicom_add_framework_test(umicom-security-authorisation-tests framework.security_authorisation tests/test_security_authorisation.c)
    umicom_add_framework_test(umicom-security-context-tests framework.security_context tests/test_security_context.c)
    umicom_add_framework_test(umicom-security-redaction-tests framework.security_redaction tests/test_security_redaction.c)
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

    umicom_add_framework_test(umicom-ai-model-tests framework.ai_model tests/test_ai_model.c)
    umicom_add_framework_test(umicom-ai-prompt-context-tests framework.ai_prompt_context tests/test_ai_prompt_context.c)
    umicom_add_framework_test(umicom-ai-retrieval-tests framework.ai_retrieval tests/test_ai_retrieval.c)
    umicom_add_framework_test(umicom-ai-embedding-tests framework.ai_embedding tests/test_ai_embedding.c)
    umicom_add_framework_test(umicom-ai-tools-policy-tests framework.ai_tools_policy tests/test_ai_tools_policy.c)
    umicom_add_framework_test(umicom-ai-provider-tests framework.ai_provider tests/test_ai_provider.c)
    umicom_add_framework_test(umicom-ai-authorengine-tests framework.ai_authorengine tests/test_ai_authorengine.c)
    umicom_add_framework_test(umicom-helix-fitness-tests framework.helix_fitness tests/test_helix_fitness.c)
    umicom_add_framework_test(umicom-helix-runtime-tests framework.helix_runtime tests/test_helix_runtime.c)
    umicom_add_framework_test(umicom-helix-release-gate-tests framework.helix_release_gate tests/test_helix_release_gate.c)
    umicom_add_framework_test(umicom-helix-memory-tests framework.helix_memory tests/test_helix_memory.c)
endif()

# -----------------------------------------------------------------------------
# Installation and CMake package export
# -----------------------------------------------------------------------------
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY templates/ DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/templates)

install(TARGETS
    umicom_base
    umicom_diagnostics
    umicom_messaging
    umicom_data
    umicom_platform
    umicom_security
    umicom_runtime
    umicom_resilience
    umicom_toolchain
    umicom_repository
    umicom_plugin
    umicom_scaffold
    umicom_ui
    umicom_ui_headless
    umicom_build
    umicom_testing
    umicom_terminal
    umicom_protocol
    umicom_vcs
    umicom_declarative
    umicom_designer
    umicom_web
    umicom_browser
    umicom_chart
    umicom_frontend
    umicom_ai
    umicom_helix
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
