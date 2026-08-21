/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_end_to_end_persistence.c
 *
 * PURPOSE:
 *   Verify an end-to-end semantic layout lifecycle through service, store adapter, session checkpoint, sharing, lease and backup evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_complete_lifecycle(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDataServiceConfig config =
        umi_workbench_layout_data_service_config_default();
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.end-to-end");
    UmiWorkbenchLayoutDocument loaded;
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.end-to-end", "layout.end-to-end");
    UmiWorkbenchLayoutGrant grant = test_create_grant(
        "grant-end-to-end", "layout.end-to-end", "reviewer",
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW));
    UmiWorkbenchLayoutLease lease;
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    UmiWorkbenchLayoutBackupManifest manifest;
    uint64_t layout_revision = 0U;
    uint64_t session_revision = 0U;
    char *backup;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    test_copy_text(config.replica_id, sizeof(config.replica_id),
                   "replica-end-to-end");
    test_copy_text(config.actor_id, sizeof(config.actor_id),
                   "developer");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        server, &config, &service));
    TEST_STATUS_OK(umi_workbench_layout_data_service_start(
        service, 1000U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        service, document, 0U, "correlation-save", 1100U,
        &layout_revision));
    TEST_REQUIRE(layout_revision == 1U,
                 "layout revision assigned");
    TEST_STATUS_OK(umi_workbench_layout_data_service_share_layout(
        service, &grant, "correlation-share", 1200U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_acquire_lease(
        service, "layout.end-to-end", "developer", "client-a",
        1300U, &lease));
    TEST_REQUIRE(lease.fencing_token > 0U,
                 "edit lease acquired");
    TEST_STATUS_OK(umi_workbench_layout_data_service_checkpoint_session(
        service, &session, 0U, &session_revision));
    TEST_REQUIRE(session_revision == 1U,
                 "session checkpointed");
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        service, "layout.end-to-end", &loaded));
    TEST_REQUIRE(loaded.node_count == document->node_count,
                 "semantic layout recovered");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        service, 1500U, &snapshot));
    TEST_REQUIRE(snapshot.layout_count == 1U,
                 "snapshot layout count");
    TEST_REQUIRE(snapshot.session_count == 1U,
                 "snapshot session count");
    TEST_REQUIRE(snapshot.metrics.save_count >= 1U,
                 "snapshot save evidence");
    TEST_REQUIRE(umi_workbench_layout_data_service_backup(
        service, "backup-end-to-end", 1600U,
        NULL, 0U, &sizing) == UMI_STATUS_CAPACITY_EXCEEDED,
        "service backup reports capacity");
    backup = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(backup != NULL, "allocate backup");
    TEST_STATUS_OK(umi_workbench_layout_data_service_backup(
        service, "backup-end-to-end", 1600U,
        backup, sizing.bytes_required + 1U, &written));
    TEST_STATUS_OK(umi_workbench_layout_backup_validate(
        backup, written.bytes_written, &manifest));
    TEST_REQUIRE(manifest.record_count > 0U,
                 "backup contains records");
    free(backup);
    free(document);
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

static int test_stale_update_preserves_authority(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.stale");
    UmiWorkbenchLayoutDocument loaded;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        server, NULL, &service));
    TEST_STATUS_OK(umi_workbench_layout_data_service_start(
        service, 1000U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        service, document, 0U, "create", 1100U, &revision));
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document, "Stale Candidate", "development",
        "This candidate must not replace the authoritative version."));
    TEST_REQUIRE(umi_workbench_layout_data_service_save_layout(
        service, document, 7U, "stale", 1200U, &revision) ==
        UMI_STATUS_INVALID_STATE, "stale update rejected");
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        service, "layout.stale", &loaded));
    TEST_REQUIRE(strcmp(loaded.name, "Development Workbench") == 0,
                 "authoritative content preserved");
    free(document);
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_complete_lifecycle() == 0,
                 "complete lifecycle");
    TEST_REQUIRE(test_stale_update_preserves_authority() == 0,
                 "stale authority");
    return 0;
}
