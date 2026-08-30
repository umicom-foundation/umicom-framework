/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_sync_cursor.c
 *
 * PURPOSE:
 *   Verify per-peer synchronisation cursors, attempts, failures, success evidence and monotonic sequence progress.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static UmiWorkbenchLayoutSyncCursor make_cursor(void)
{
    UmiWorkbenchLayoutSyncCursor cursor;
    (void)memset(&cursor, 0, sizeof(cursor));
    cursor.structure_size = sizeof(cursor);
    test_copy_text(cursor.cursor_id, sizeof(cursor.cursor_id),
                   "cursor-local-remote");
    test_copy_text(cursor.replica_id, sizeof(cursor.replica_id), "local");
    test_copy_text(cursor.peer_id, sizeof(cursor.peer_id), "remote");
    cursor.revision = 1U;
    return cursor;
}

static int test_cursor_codec(void)
{
    UmiWorkbenchLayoutSyncCursor source = make_cursor();
    UmiWorkbenchLayoutSyncCursor decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    source.last_pulled_sequence = 10U;
    source.last_pushed_sequence = 20U;
    source.last_success_at_ms = 3000U;
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_decode(
        value, &decoded));
    TEST_REQUIRE(strcmp(decoded.peer_id, "remote") == 0,
                 "peer round trip");
    TEST_REQUIRE(decoded.last_pulled_sequence == 10U,
                 "pull sequence round trip");
    TEST_REQUIRE(decoded.last_pushed_sequence == 20U,
                 "push sequence round trip");
    return 0;
}

static int test_attempt_and_success(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSyncCursorRepository repository;
    UmiWorkbenchLayoutSyncCursor cursor = make_cursor();
    UmiWorkbenchLayoutSyncCursor loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_save(
        &repository, &cursor));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_record_attempt(
        &repository, &cursor, 1000U, "peer unavailable"));
    TEST_REQUIRE(cursor.last_attempt_at_ms == 1000U,
                 "attempt time recorded");
    TEST_REQUIRE(strcmp(cursor.last_error, "peer unavailable") == 0,
                 "attempt error recorded");
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_record_success(
        &repository, &cursor, 11U, 7U, 2000U));
    TEST_REQUIRE(cursor.last_pulled_sequence == 11U,
                 "pulled sequence advanced");
    TEST_REQUIRE(cursor.last_pushed_sequence == 7U,
                 "pushed sequence advanced");
    TEST_REQUIRE(cursor.last_success_at_ms == 2000U,
                 "success time recorded");
    TEST_REQUIRE(cursor.last_error[0] == '\0',
                 "success clears error");
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_load(
        &repository, "local", "cursor-local-remote", 0U, &loaded));
    TEST_REQUIRE(loaded.last_pulled_sequence == 11U,
                 "success persisted");
    umi_data_server_destroy(server);
    return 0;
}

static int test_sequence_never_moves_backwards(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSyncCursorRepository repository;
    UmiWorkbenchLayoutSyncCursor cursor = make_cursor();
    TEST_REQUIRE(server != NULL, "create server");
    cursor.last_pulled_sequence = 20U;
    cursor.last_pushed_sequence = 30U;
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_save(
        &repository, &cursor));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_record_success(
        &repository, &cursor, 10U, 15U, 2000U));
    TEST_REQUIRE(cursor.last_pulled_sequence == 20U,
                 "pull cursor remains monotonic");
    TEST_REQUIRE(cursor.last_pushed_sequence == 30U,
                 "push cursor remains monotonic");
    umi_data_server_destroy(server);
    return 0;
}

static int test_cursor_listing(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSyncCursorRepository repository;
    UmiWorkbenchLayoutSyncCursor first = make_cursor();
    UmiWorkbenchLayoutSyncCursor second = make_cursor();
    UmiWorkbenchLayoutSyncCursor results[4];
    size_t count = 0U;
    size_t total = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    test_copy_text(second.cursor_id, sizeof(second.cursor_id),
                   "cursor-local-backup");
    test_copy_text(second.peer_id, sizeof(second.peer_id), "backup");
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_save(
        &repository, &first));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_save(
        &repository, &second));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_list(
        &repository, "local", NULL, NULL,
        results, 4U, &count, &total));
    TEST_REQUIRE(count == 2U && total == 2U,
                 "replica cursors listed");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_cursor_codec() == 0, "cursor codec");
    TEST_REQUIRE(test_attempt_and_success() == 0,
                 "attempt success");
    TEST_REQUIRE(test_sequence_never_moves_backwards() == 0,
                 "monotonic cursor");
    TEST_REQUIRE(test_cursor_listing() == 0, "cursor listing");
    return 0;
}
