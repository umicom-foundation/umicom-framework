/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_service_backup_restore.c
 *
 * PURPOSE:
 *   Verify service-level backup and restore between independent Data Server instances while preserving semantic layouts and sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

/*
 * Exercise started service and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiWorkbenchLayoutDataService *started_service(
    UmiDataServer *server,
    const char *replica_id)
{
    UmiWorkbenchLayoutDataServiceConfig config =
        umi_workbench_layout_data_service_config_default();
    UmiWorkbenchLayoutDataService *service = NULL;
    test_copy_text(config.replica_id, sizeof(config.replica_id), replica_id);
    test_copy_text(config.actor_id, sizeof(config.actor_id), "developer");
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_data_service_create(
            server, &config, &service) != UMI_STATUS_OK) {
        return NULL;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_data_service_start(service, 1000U) !=
        UMI_STATUS_OK) {
        umi_workbench_layout_data_service_destroy(service);
        return NULL;
    }
    return service;
}

/*
 * Exercise test cross server restore and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_cross_server_restore(void)
{
    UmiDataServer *source_server = test_create_data_server();
    UmiDataServer *target_server = test_create_data_server();
    UmiWorkbenchLayoutDataService *source;
    UmiWorkbenchLayoutDataService *target;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.restore-service");
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.restore-service", "layout.restore-service");
    UmiWorkbenchLayoutDocument restored_document;
    UmiWorkbenchLayoutDataServiceSnapshot target_snapshot;
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    UmiWorkbenchLayoutRestoreOptions options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult restored;
    uint64_t revision = 0U;
    char *backup;
    TEST_REQUIRE(source_server != NULL && target_server != NULL &&
                 document != NULL, "create fixtures");
    source = started_service(source_server, "source");
    target = started_service(target_server, "target");
    TEST_REQUIRE(source != NULL && target != NULL, "create services");
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        source, document, 0U, "save", 1100U, &revision));
    TEST_STATUS_OK(umi_workbench_layout_data_service_checkpoint_session(
        source, &session, 0U, &revision));
    TEST_REQUIRE(umi_workbench_layout_data_service_backup(
        source, "backup-service", 1200U,
        NULL, 0U, &sizing) == UMI_STATUS_CAPACITY_EXCEEDED,
        "backup sizing");
    backup = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(backup != NULL, "allocate backup");
    TEST_STATUS_OK(umi_workbench_layout_data_service_backup(
        source, "backup-service", 1200U,
        backup, sizing.bytes_required + 1U, &written));
    TEST_STATUS_OK(umi_workbench_layout_data_service_restore(
        target, backup, written.bytes_written, &options, &restored));
    TEST_REQUIRE(restored.restored_count > 0U,
                 "records restored through service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        target, "layout.restore-service", &restored_document));
    TEST_REQUIRE(restored_document.node_count == document->node_count,
                 "restored semantic document");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        target, 1300U, &target_snapshot));
    TEST_REQUIRE(target_snapshot.layout_count == 1U,
                 "restored layout counted");
    TEST_REQUIRE(target_snapshot.session_count == 1U,
                 "restored session counted");
    free(backup);
    free(document);
    umi_workbench_layout_data_service_destroy(source);
    umi_workbench_layout_data_service_destroy(target);
    umi_data_server_destroy(source_server);
    umi_data_server_destroy(target_server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_cross_server_restore() == 0,
                 "cross server restore");
    return 0;
}
