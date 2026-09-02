/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_record_codecs.c
 *
 * PURPOSE:
 *   Verify round-trip encoding of all remaining collaboration, queue and synchronisation record families through one durable field format.
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
 * Exercise test outbox round trip and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_outbox_round_trip(void)
{
    UmiWorkbenchLayoutOutboxEntry source;
    UmiWorkbenchLayoutOutboxEntry decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    (void)memset(&source, 0, sizeof(source));
    source.structure_size = sizeof(source);
    test_copy_text(source.entry_id, sizeof(source.entry_id), "outbox-codec");
    test_copy_text(source.topic, sizeof(source.topic), "layout.changed");
    test_copy_text(source.aggregate_id,
                   sizeof(source.aggregate_id), "layout.codec");
    source.state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_RETRY_WAIT;
    source.sequence = 8U;
    source.attempt_count = 2U;
    source.maximum_attempts = 5U;
    TEST_STATUS_OK(umi_workbench_layout_outbox_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_outbox_decode(value, &decoded));
    TEST_REQUIRE(decoded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_RETRY_WAIT,
        "outbox state round trip");
    TEST_REQUIRE(decoded.sequence == 8U, "outbox sequence");
    return 0;
}

/*
 * Exercise test conflict round trip and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_conflict_round_trip(void)
{
    UmiWorkbenchLayoutConflict source;
    UmiWorkbenchLayoutConflict decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    (void)memset(&source, 0, sizeof(source));
    source.structure_size = sizeof(source);
    test_copy_text(source.conflict_id,
                   sizeof(source.conflict_id), "conflict-codec");
    test_copy_text(source.layout_id,
                   sizeof(source.layout_id), "layout.codec");
    test_copy_text(source.local_replica_id,
                   sizeof(source.local_replica_id), "local");
    test_copy_text(source.remote_replica_id,
                   sizeof(source.remote_replica_id), "remote");
    source.kind = UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_MISSING_BASE;
    source.state = UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN;
    source.local_revision = 4U;
    source.remote_revision = 9U;
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_decode(
        value, &decoded));
    TEST_REQUIRE(decoded.kind ==
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_MISSING_BASE,
        "conflict kind round trip");
    TEST_REQUIRE(decoded.remote_revision == 9U,
        "conflict revision round trip");
    return 0;
}

/*
 * Exercise test cursor round trip and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_cursor_round_trip(void)
{
    UmiWorkbenchLayoutSyncCursor source;
    UmiWorkbenchLayoutSyncCursor decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    (void)memset(&source, 0, sizeof(source));
    source.structure_size = sizeof(source);
    test_copy_text(source.cursor_id, sizeof(source.cursor_id), "cursor-codec");
    test_copy_text(source.replica_id, sizeof(source.replica_id), "local");
    test_copy_text(source.peer_id, sizeof(source.peer_id), "remote");
    source.last_pulled_sequence = 4U;
    source.last_pushed_sequence = 7U;
    source.revision = 2U;
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_decode(
        value, &decoded));
    TEST_REQUIRE(decoded.last_pulled_sequence == 4U,
                 "cursor pull round trip");
    TEST_REQUIRE(decoded.last_pushed_sequence == 7U,
                 "cursor push round trip");
    return 0;
}

/*
 * Exercise test offline round trip and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_offline_round_trip(void)
{
    UmiWorkbenchLayoutOfflineOperation source;
    UmiWorkbenchLayoutOfflineOperation decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    (void)memset(&source, 0, sizeof(source));
    source.structure_size = sizeof(source);
    test_copy_text(source.operation_id,
                   sizeof(source.operation_id), "offline-codec");
    test_copy_text(source.layout_id,
                   sizeof(source.layout_id), "layout.codec");
    test_copy_text(source.actor_id,
                   sizeof(source.actor_id), "developer");
    source.change_kind = UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED;
    source.state = UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT;
    source.maximum_attempts = 3U;
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_decode(
        value, &decoded));
    TEST_REQUIRE(decoded.change_kind ==
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED,
        "offline change kind round trip");
    TEST_REQUIRE(decoded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT,
        "offline state round trip");
    return 0;
}

/*
 * Exercise test presence round trip and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_presence_round_trip(void)
{
    UmiWorkbenchLayoutPresence source;
    UmiWorkbenchLayoutPresence decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    (void)memset(&source, 0, sizeof(source));
    source.structure_size = sizeof(source);
    test_copy_text(source.presence_id,
                   sizeof(source.presence_id), "presence-codec");
    test_copy_text(source.layout_id,
                   sizeof(source.layout_id), "layout.codec");
    test_copy_text(source.user_id,
                   sizeof(source.user_id), "developer");
    test_copy_text(source.client_id,
                   sizeof(source.client_id), "client-a");
    test_copy_text(source.active_node_id,
                   sizeof(source.active_node_id), "editor");
    source.editing = true;
    source.synchronising = true;
    TEST_STATUS_OK(umi_workbench_layout_presence_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_decode(
        value, &decoded));
    TEST_REQUIRE(decoded.editing && decoded.synchronising,
                 "presence activity round trip");
    TEST_REQUIRE(strcmp(decoded.active_node_id, "editor") == 0,
                 "presence node round trip");
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_outbox_round_trip() == 0,
                 "outbox round trip");
    TEST_REQUIRE(test_conflict_round_trip() == 0,
                 "conflict round trip");
    TEST_REQUIRE(test_cursor_round_trip() == 0,
                 "cursor round trip");
    TEST_REQUIRE(test_offline_round_trip() == 0,
                 "offline round trip");
    TEST_REQUIRE(test_presence_round_trip() == 0,
                 "presence round trip");
    return 0;
}
