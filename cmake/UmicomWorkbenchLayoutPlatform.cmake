#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchLayoutPlatform.cmake
#
# PURPOSE:
#   Register the Framework-owned persistent semantic workbench layout platform,
#   its public C ABI, deterministic tests and installable portable resources.
#
# ARCHITECTURE:
#   The target is toolkit-neutral. GTK4, Qt, Wt and headless frontends consume
#   immutable snapshots or project the semantic tree through adapter-specific
#   renderers. User and session persistence is accessed only through the store
#   port so a Data Server adapter can remain the authoritative implementation.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(TARGET umicom_workbench_layout)
    return()
endif()

if(NOT TARGET Umicom::base OR NOT TARGET Umicom::desktop)
    message(FATAL_ERROR
        "The workbench layout platform requires the established Framework "
        "base and desktop targets.")
endif()

add_library(umicom_workbench_layout STATIC
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/access_policy.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/browser.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/commands.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/controller.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/desktop_projection.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/diagnostics.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/diff.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/document.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/events.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/history.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/json.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/memory_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/merge.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/migration.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/monitor_topology.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/node.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/operation.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/panel_registry.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/persistence.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/perspective_registry.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/placement.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/query.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/recovery.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/responsive.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/service.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/session.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/sharing.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/snapshot.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/store.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/template_registry.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/transaction.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/traversal.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/types.c"
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout/validation.c"
)
add_library(Umicom::workbench_layout ALIAS umicom_workbench_layout)
set_target_properties(umicom_workbench_layout PROPERTIES
    EXPORT_NAME workbench_layout
)

target_include_directories(umicom_workbench_layout
    PUBLIC
        $<BUILD_INTERFACE:${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/src/workbench_layout"
)
target_link_libraries(umicom_workbench_layout
    PUBLIC
        Umicom::base
        Umicom::desktop
)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_layout)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_layout)
endif()

# The aggregate Framework target exposes this capability to first-party product
# profiles while bounded consumers may link Umicom::workbench_layout directly.
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_layout
    )
endif()

if(BUILD_TESTING)
    function(umicom_add_workbench_layout_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable(
            "${target}"
            "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/${source}"
        )
        target_include_directories("${target}" PRIVATE
            "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/tests/workbench_layout"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_layout
        )
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_add_workbench_layout_test(
        umicom-workbench-layout-document-validation-test
        framework.workbench_layout.document_validation
        tests/workbench_layout/test_document_validation.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-traversal-query-test
        framework.workbench_layout.traversal_query
        tests/workbench_layout/test_traversal_query.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-operations-history-test
        framework.workbench_layout.operations_history
        tests/workbench_layout/test_operations_history.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-diff-merge-test
        framework.workbench_layout.diff_merge
        tests/workbench_layout/test_diff_merge.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-json-sharing-test
        framework.workbench_layout.json_sharing
        tests/workbench_layout/test_json_sharing.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-templates-registries-test
        framework.workbench_layout.templates_registries
        tests/workbench_layout/test_templates_registries.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-monitor-responsive-test
        framework.workbench_layout.monitor_responsive
        tests/workbench_layout/test_monitor_responsive.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-session-recovery-test
        framework.workbench_layout.session_recovery
        tests/workbench_layout/test_session_recovery.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-store-persistence-test
        framework.workbench_layout.store_persistence
        tests/workbench_layout/test_store_persistence.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-service-controller-test
        framework.workbench_layout.service_controller
        tests/workbench_layout/test_service_controller.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-snapshot-diagnostics-projection-test
        framework.workbench_layout.snapshot_diagnostics_projection
        tests/workbench_layout/test_snapshot_diagnostics_projection.c
    )
    umicom_add_workbench_layout_test(
        umicom-workbench-layout-events-commands-test
        framework.workbench_layout.events_commands
        tests/workbench_layout/test_events_commands.c
    )
endif()

install(TARGETS umicom_workbench_layout
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/include/umicom/workbench_layout"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)
install(FILES
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/resources/schemas/workbench-layout.schema.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/schemas"
)
install(FILES
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/resources/workbench-layout-defaults.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources"
)
install(DIRECTORY
    "${UMICOM_WORKBENCH_LAYOUT_FRAMEWORK_ROOT}/resources/layouts/templates/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/layouts/templates"
    FILES_MATCHING PATTERN "*.umilayout"
)

# Data Server persistence and synchronisation extend the semantic platform
# without changing portable .umilayout ownership or frontend-neutral models.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchLayoutDataPlatform.cmake")

message(STATUS
    "Umicom persistent semantic workbench layout platform enabled")
