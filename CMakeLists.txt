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
    src/messaging/event_bus.c
    src/messaging/command_bus.c
    src/messaging/query_bus.c
    src/messaging/journal.c
)
add_library(Umicom::messaging ALIAS umicom_messaging)
set_target_properties(umicom_messaging PROPERTIES EXPORT_NAME messaging)
target_link_libraries(umicom_messaging PUBLIC Umicom::base)
target_include_directories(umicom_messaging PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_messaging)
umicom_apply_sanitizers(umicom_messaging)

# -----------------------------------------------------------------------------
# Data Server library
# -----------------------------------------------------------------------------
find_package(SQLite3 QUIET)

add_library(umicom_data STATIC
    src/data/data_server.c
)
add_library(Umicom::data ALIAS umicom_data)
set_target_properties(umicom_data PROPERTIES EXPORT_NAME data)
target_link_libraries(umicom_data PUBLIC Umicom::base)
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
target_include_directories(umicom_data PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
)
add_library(Umicom::plugin ALIAS umicom_plugin)
set_target_properties(umicom_plugin PROPERTIES EXPORT_NAME plugin)
target_link_libraries(umicom_plugin PUBLIC
    Umicom::base
    Umicom::runtime
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
    Umicom::toolchain
    Umicom::repository
    Umicom::plugin
    Umicom::scaffold
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
    umicom_toolchain
    umicom_repository
    umicom_plugin
    umicom_scaffold
    umicom_framework
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

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
