/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_fault_evidence.c
 *
 * PURPOSE:
 *   Verify that failed writes, sync attempts and health evaluation retain bounded diagnostic evidence without corrupting authoritative state.
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
 * Exercise test service failure metrics and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_service_failure_metrics(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.failure");
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        server, NULL, &service));
    TEST_STATUS_OK(umi_workbench_layout_data_service_start(
        service, 1000U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        service, document, 0U, "create", 1100U, &revision));
    TEST_REQUIRE(umi_workbench_layout_data_service_save_layout(
        service, document, 99U, "stale", 1200U, &revision) ==
        UMI_STATUS_INVALID_STATE, "stale write fails");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        service, 1300U, &snapshot));
    TEST_REQUIRE(snapshot.metrics.save_count >= 2U,
                 "failed save included in metrics");
    TEST_REQUIRE(snapshot.health.last_failure_at_ms >= 1200U,
                 "failure time retained");
    free(document);
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test cursor failure evidence and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_cursor_failure_evidence(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSyncCursorRepository repository;
    UmiWorkbenchLayoutSyncCursor cursor;
    UmiWorkbenchLayoutSyncCursor loaded;
    TEST_REQUIRE(server != NULL, "create server");
    (void)memset(&cursor, 0, sizeof(cursor));
    cursor.structure_size = sizeof(cursor);
    test_copy_text(cursor.cursor_id, sizeof(cursor.cursor_id), "cursor-fail");
    test_copy_text(cursor.replica_id, sizeof(cursor.replica_id), "local");
    test_copy_text(cursor.peer_id, sizeof(cursor.peer_id), "remote");
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_save(
        &repository, &cursor));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_record_attempt(
        &repository, &cursor, 2000U,
        "remote endpoint unavailable"));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_load(
        &repository, "local", "cursor-fail", 0U, &loaded));
    TEST_REQUIRE(strcmp(loaded.last_error,
        "remote endpoint unavailable") == 0,
        "sync error persisted");
    TEST_REQUIRE(loaded.last_attempt_at_ms == 2000U,
                 "sync attempt time persisted");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test outbox error capacity and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_outbox_error_capacity(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOutboxRepository repository;
    UmiWorkbenchLayoutOutboxEntry entry;
    UmiWorkbenchLayoutOutboxEntry claimed;
    char error[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY + 64U];
    TEST_REQUIRE(server != NULL, "create server");
    (void)memset(&entry, 0, sizeof(entry));
    entry.structure_size = sizeof(entry);
    test_copy_text(entry.entry_id, sizeof(entry.entry_id), "outbox-fault");
    test_copy_text(entry.topic, sizeof(entry.topic), "layout.changed");
    test_copy_text(entry.aggregate_id,
                   sizeof(entry.aggregate_id), "layout.failure");
    entry.state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_PENDING;
    entry.sequence = 1U;
    entry.available_at_ms = 1000U;
    entry.maximum_attempts = 2U;
    (void)memset(error, 'x', sizeof(error) - 1U);
    error[sizeof(error) - 1U] = '\0';
    TEST_STATUS_OK(umi_workbench_layout_outbox_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_outbox_save(
        &repository, &entry));
    TEST_STATUS_OK(umi_workbench_layout_outbox_claim_available(
        &repository, 1000U, 100U, &claimed));
    TEST_REQUIRE(umi_workbench_layout_outbox_mark_failed(
        &repository, &claimed, 1100U, 100U, error) ==
        UMI_STATUS_CAPACITY_EXCEEDED,
        "oversized diagnostic rejected without truncation");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_service_failure_metrics() == 0,
                 "service failure metrics");
    TEST_REQUIRE(test_cursor_failure_evidence() == 0,
                 "cursor failure evidence");
    TEST_REQUIRE(test_outbox_error_capacity() == 0,
                 "outbox error capacity");
    return 0;
}
