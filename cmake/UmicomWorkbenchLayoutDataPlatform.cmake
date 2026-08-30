#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchLayoutDataPlatform.cmake
#
# PURPOSE:
#   Register Data Server-backed semantic layout persistence, replication,
#   synchronisation, recovery, collaboration and operational evidence.
#
# ARCHITECTURE:
#   The Data Server is the authoritative owner of Umicom layout and session
#   state. Portable .umilayout documents remain import/export and source-
#   control artefacts. This library provides the persistent store adapter,
#   typed repositories, optimistic revisions, leases, outbox, change feed,
#   offline replay, conflict evidence, backup and migration services.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(TARGET umicom_workbench_layout_data)
    return()
endif()

if(NOT TARGET Umicom::workbench_layout OR NOT TARGET Umicom::data)
    message(FATAL_ERROR
        "The workbench layout data platform requires the established "
        "workbench layout and Data Server targets.")
endif()

add_library(umicom_workbench_layout_data STATIC
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/backup.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/change_feed.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/chunk_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/conflict_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/controller.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/data_server_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/document_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/grant_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/health.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/key_codec.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/lease_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/metrics.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/migration_runner.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/migration_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/offline_queue.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/outbox.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/presence_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/reconciliation.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/record_repository.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/restore.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/retention.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/revision_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/schema_catalogue.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/service.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/session_store.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/sync_cursor.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/sync_engine.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/sync_plan.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/types.c"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data/value_codec.c"
)
add_library(Umicom::workbench_layout_data ALIAS umicom_workbench_layout_data)
set_target_properties(umicom_workbench_layout_data PROPERTIES
    EXPORT_NAME workbench_layout_data
)

target_include_directories(umicom_workbench_layout_data
    PUBLIC
        $<BUILD_INTERFACE:${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/src/workbench_layout_data"
)
target_link_libraries(umicom_workbench_layout_data
    PUBLIC
        Umicom::workbench_layout
        Umicom::data
)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_layout_data)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_layout_data)
endif()

# The complete Framework target exposes persistent layouts while bounded
# consumers may link the smaller Umicom::workbench_layout_data target.
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_layout_data
    )
endif()

if(BUILD_TESTING)
    function(umicom_add_workbench_layout_data_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable(
            "${target}"
            "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/${source}"
        )
        target_include_directories("${target}" PRIVATE
            "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/tests/workbench_layout_data"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_layout_data
        )
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-backend-contract-test
        framework.workbench_layout_data.backend_contract
        tests/workbench_layout_data/test_backend_contract.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-backup-test
        framework.workbench_layout_data.backup
        tests/workbench_layout_data/test_backup.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-capacity-boundaries-test
        framework.workbench_layout_data.capacity_boundaries
        tests/workbench_layout_data/test_capacity_boundaries.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-change-feed-test
        framework.workbench_layout_data.change_feed
        tests/workbench_layout_data/test_change_feed.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-chunk-store-test
        framework.workbench_layout_data.chunk_store
        tests/workbench_layout_data/test_chunk_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-conflict-store-test
        framework.workbench_layout_data.conflict_store
        tests/workbench_layout_data/test_conflict_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-controller-test
        framework.workbench_layout_data.controller
        tests/workbench_layout_data/test_controller.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-data-server-store-test
        framework.workbench_layout_data.data_server_store
        tests/workbench_layout_data/test_data_server_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-document-store-test
        framework.workbench_layout_data.document_store
        tests/workbench_layout_data/test_document_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-end-to-end-persistence-test
        framework.workbench_layout_data.end_to_end_persistence
        tests/workbench_layout_data/test_end_to_end_persistence.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-fault-evidence-test
        framework.workbench_layout_data.fault_evidence
        tests/workbench_layout_data/test_fault_evidence.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-grant-store-test
        framework.workbench_layout_data.grant_store
        tests/workbench_layout_data/test_grant_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-health-test
        framework.workbench_layout_data.health
        tests/workbench_layout_data/test_health.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-key-value-codec-test
        framework.workbench_layout_data.key_value_codec
        tests/workbench_layout_data/test_key_value_codec.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-lease-store-test
        framework.workbench_layout_data.lease_store
        tests/workbench_layout_data/test_lease_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-metrics-test
        framework.workbench_layout_data.metrics
        tests/workbench_layout_data/test_metrics.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-migration-runner-test
        framework.workbench_layout_data.migration_runner
        tests/workbench_layout_data/test_migration_runner.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-migration-store-test
        framework.workbench_layout_data.migration_store
        tests/workbench_layout_data/test_migration_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-offline-queue-test
        framework.workbench_layout_data.offline_queue
        tests/workbench_layout_data/test_offline_queue.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-outbox-test
        framework.workbench_layout_data.outbox
        tests/workbench_layout_data/test_outbox.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-presence-store-test
        framework.workbench_layout_data.presence_store
        tests/workbench_layout_data/test_presence_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-reconciliation-test
        framework.workbench_layout_data.reconciliation
        tests/workbench_layout_data/test_reconciliation.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-record-codecs-test
        framework.workbench_layout_data.record_codecs
        tests/workbench_layout_data/test_record_codecs.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-record-repository-test
        framework.workbench_layout_data.record_repository
        tests/workbench_layout_data/test_record_repository.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-replica-recovery-test
        framework.workbench_layout_data.replica_recovery
        tests/workbench_layout_data/test_replica_recovery.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-restore-test
        framework.workbench_layout_data.restore
        tests/workbench_layout_data/test_restore.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-retention-test
        framework.workbench_layout_data.retention
        tests/workbench_layout_data/test_retention.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-revision-store-test
        framework.workbench_layout_data.revision_store
        tests/workbench_layout_data/test_revision_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-schema-catalogue-test
        framework.workbench_layout_data.schema_catalogue
        tests/workbench_layout_data/test_schema_catalogue.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-service-test
        framework.workbench_layout_data.service
        tests/workbench_layout_data/test_service.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-service-backup-restore-test
        framework.workbench_layout_data.service_backup_restore
        tests/workbench_layout_data/test_service_backup_restore.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-session-store-test
        framework.workbench_layout_data.session_store
        tests/workbench_layout_data/test_session_store.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-store-query-paging-test
        framework.workbench_layout_data.store_query_paging
        tests/workbench_layout_data/test_store_query_paging.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-sync-cursor-test
        framework.workbench_layout_data.sync_cursor
        tests/workbench_layout_data/test_sync_cursor.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-sync-engine-test
        framework.workbench_layout_data.sync_engine
        tests/workbench_layout_data/test_sync_engine.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-sync-plan-test
        framework.workbench_layout_data.sync_plan
        tests/workbench_layout_data/test_sync_plan.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-sync-policy-test
        framework.workbench_layout_data.sync_policy
        tests/workbench_layout_data/test_sync_policy.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-transaction-atomicity-test
        framework.workbench_layout_data.transaction_atomicity
        tests/workbench_layout_data/test_transaction_atomicity.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-types-test
        framework.workbench_layout_data.types
        tests/workbench_layout_data/test_types.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-collaboration-access-lifecycle-test
        framework.workbench_layout_data.collaboration_access_lifecycle
        tests/workbench_layout_data/test_collaboration_access_lifecycle.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-backup-restore-atomicity-test
        framework.workbench_layout_data.backup_restore_atomicity
        tests/workbench_layout_data/test_backup_restore_atomicity.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-change-sequence-invariants-test
        framework.workbench_layout_data.change_sequence_invariants
        tests/workbench_layout_data/test_change_sequence_invariants.c
    )
    umicom_add_workbench_layout_data_test(
        umicom-workbench-layout-data-service-recovery-lifecycle-test
        framework.workbench_layout_data.service_recovery_lifecycle
        tests/workbench_layout_data/test_service_recovery_lifecycle.c
    )
endif()

install(TARGETS umicom_workbench_layout_data
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/include/umicom/workbench_layout_data"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)
install(FILES
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/resources/workbench-layout-data-defaults.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources"
)
install(FILES
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/resources/schemas/workbench-layout-data-defaults.schema.json"
    "${UMICOM_WORKBENCH_LAYOUT_DATA_FRAMEWORK_ROOT}/resources/schemas/workbench-layout-sync.schema.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/schemas"
)

# The visual Layout Browser and designer consume the semantic and persistent
# layout targets declared above. Keeping this include here makes Framework
# independently buildable without a product repository patching its targets.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchDesignerPlatform.cmake")

message(STATUS
    "Umicom Data Server workbench layout persistence enabled")
