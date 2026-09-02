/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_service_recovery_lifecycle.c
 *
 * PURPOSE:
 *   Verify service restart, session recovery, stale-write protection and
 *   cross-replica backup restoration through the authoritative Data Server.
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
 * Exercise create service and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiWorkbenchLayoutDataService *create_service(
    UmiDataServer *server,
    const char *replica_id,
    uint64_t now_ms)
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
    if (umi_workbench_layout_data_service_start(service, now_ms) !=
        UMI_STATUS_OK) {
        umi_workbench_layout_data_service_destroy(service);
        return NULL;
    }
    return service;
}

/*
 * Exercise test restart preserves layout and session and return a clear result when the
 * behaviour no longer matches its contract.
 */
static int test_restart_preserves_layout_and_session(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *first;
    UmiWorkbenchLayoutDataService *second;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.recovery");
    UmiWorkbenchLayoutDocument loaded;
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.recovery", "layout.recovery");
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    uint64_t layout_revision = 0U;
    uint64_t session_revision = 0U;

    TEST_REQUIRE(server != NULL && document != NULL,
                 "create restart fixtures");
    first = create_service(server, "replica-restart", 1000U);
    TEST_REQUIRE(first != NULL, "start first service generation");
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        first, document, 0U, "save-recovery", 1500U,
        &layout_revision));
    TEST_STATUS_OK(umi_workbench_layout_data_service_checkpoint_session(
        first, &session, 0U, &session_revision));
    TEST_REQUIRE(layout_revision == 1U && session_revision == 1U,
                 "initial revisions assigned");
    TEST_STATUS_OK(umi_workbench_layout_data_service_stop(first, 2000U));
    umi_workbench_layout_data_service_destroy(first);

    second = create_service(server, "replica-restart", 3000U);
    TEST_REQUIRE(second != NULL, "restart service on same Data Server");
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        second, "layout.recovery", &loaded));
    TEST_REQUIRE(loaded.version.revision == layout_revision,
                 "layout revision survives service restart");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        second, 3200U, &snapshot));
    TEST_REQUIRE(snapshot.layout_count == 1U,
                 "restarted service discovers layout");
    TEST_REQUIRE(snapshot.session_count == 1U,
                 "restarted service discovers recovery session");
    TEST_REQUIRE(snapshot.started && snapshot.accepting_writes,
                 "restarted service is operational");

    umi_workbench_layout_data_service_destroy(second);
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test stale write is rejected without mutation and return a clear result when
 * the behaviour no longer matches its contract.
 */
static int test_stale_write_is_rejected_without_mutation(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.stale-write");
    UmiWorkbenchLayoutDocument loaded;
    uint64_t first_revision = 0U;
    uint64_t second_revision = 0U;

    TEST_REQUIRE(server != NULL && document != NULL,
                 "create stale-write fixtures");
    service = create_service(server, "replica-stale", 1000U);
    TEST_REQUIRE(service != NULL, "start stale-write service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        service, document, 0U, "create", 1500U, &first_revision));
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document, "Updated Workbench", "development",
        "Updated by the current revision holder."));
    umi_workbench_layout_document_refresh_hash(document);
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        service, document, first_revision, "update", 1600U,
        &second_revision));
    TEST_REQUIRE(second_revision == first_revision + 1U,
                 "current revision advances");

    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document, "Stale Workbench", "development",
        "This stale update must not replace authoritative state."));
    umi_workbench_layout_document_refresh_hash(document);
    TEST_REQUIRE(umi_workbench_layout_data_service_save_layout(
        service, document, first_revision, "stale", 1700U,
        &first_revision) == UMI_STATUS_INVALID_STATE,
        "stale expected revision rejected");
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        service, "layout.stale-write", &loaded));
    TEST_REQUIRE(loaded.version.revision == second_revision,
                 "authoritative revision remains unchanged");
    TEST_REQUIRE(strcmp(loaded.name, "Updated Workbench") == 0,
                 "stale content does not replace current content");

    umi_workbench_layout_data_service_destroy(service);
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test cross replica recovery and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_cross_replica_recovery(void)
{
    UmiDataServer *source_server = test_create_data_server();
    UmiDataServer *target_server = test_create_data_server();
    UmiWorkbenchLayoutDataService *source;
    UmiWorkbenchLayoutDataService *target;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.cross-replica");
    UmiWorkbenchLayoutDocument loaded;
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    UmiWorkbenchLayoutRestoreOptions options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult restored;
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    uint64_t revision = 0U;
    char *backup;

    TEST_REQUIRE(source_server != NULL && target_server != NULL &&
                 document != NULL, "create replica fixtures");
    source = create_service(source_server, "replica-source", 1000U);
    target = create_service(target_server, "replica-target", 1000U);
    TEST_REQUIRE(source != NULL && target != NULL,
                 "start source and target services");
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        source, document, 0U, "replicate", 2000U, &revision));

    TEST_REQUIRE(umi_workbench_layout_data_service_backup(
        source, "backup-cross-replica", 2500U,
        NULL, 0U, &sizing) == UMI_STATUS_CAPACITY_EXCEEDED,
        "service backup reports capacity");
    backup = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(backup != NULL, "allocate service backup");
    TEST_STATUS_OK(umi_workbench_layout_data_service_backup(
        source, "backup-cross-replica", 2500U,
        backup, sizing.bytes_required + 1U, &written));
    TEST_STATUS_OK(umi_workbench_layout_data_service_restore(
        target, backup, written.bytes_written, &options, &restored));
    TEST_REQUIRE(restored.restored_count > 0U,
                 "target restores source records");
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        target, "layout.cross-replica", &loaded));
    TEST_REQUIRE(loaded.version.revision == revision,
                 "replica restores semantic layout revision");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        target, 3000U, &snapshot));
    TEST_REQUIRE(snapshot.layout_count == 1U,
                 "target snapshot reports restored layout");
    TEST_REQUIRE(snapshot.health.state !=
                     UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNAVAILABLE,
                 "restored replica remains observable");

    free(backup);
    umi_workbench_layout_data_service_destroy(source);
    umi_workbench_layout_data_service_destroy(target);
    free(document);
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
    TEST_REQUIRE(test_restart_preserves_layout_and_session() == 0,
                 "service restart preservation");
    TEST_REQUIRE(test_stale_write_is_rejected_without_mutation() == 0,
                 "stale-write protection");
    TEST_REQUIRE(test_cross_replica_recovery() == 0,
                 "cross-replica recovery");
    return 0;
}
