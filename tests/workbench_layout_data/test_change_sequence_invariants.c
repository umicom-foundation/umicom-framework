/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_change_sequence_invariants.c
 *
 * PURPOSE:
 *   Verify ordered change feeds, cursor monotonicity and acknowledgement-based
 *   filtering of deterministic synchronisation plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static UmiStatus change_matches_layout(
    const void *record_value,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutChange *change =
        (const UmiWorkbenchLayoutChange *)record_value;
    const char *layout_id = (const char *)context;
    if (change == NULL || layout_id == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches = strcmp(change->layout_id, layout_id) == 0;
    return UMI_STATUS_OK;
}

static UmiWorkbenchLayoutSyncCursor make_cursor(void)
{
    UmiWorkbenchLayoutSyncCursor cursor;
    (void)memset(&cursor, 0, sizeof(cursor));
    cursor.structure_size = sizeof(cursor);
    test_copy_text(cursor.cursor_id, sizeof(cursor.cursor_id),
                   "cursor-sequence");
    test_copy_text(cursor.replica_id, sizeof(cursor.replica_id),
                   "replica-local");
    test_copy_text(cursor.peer_id, sizeof(cursor.peer_id),
                   "replica-remote");
    return cursor;
}

static int test_feed_order_and_layout_filter(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChangeFeedRepository feed;
    UmiWorkbenchLayoutChange changes[4];
    UmiWorkbenchLayoutChange listed[8];
    size_t count = 0U;
    size_t total = 0U;

    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_change_feed_repository_init(
        &feed, server));
    changes[0] = test_create_change(
        "change-30", "layout.sequence",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        30U, 2U, 3U, 300U);
    changes[1] = test_create_change(
        "change-10", "layout.sequence",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        10U, 0U, 1U, 100U);
    changes[2] = test_create_change(
        "change-20", "layout.sequence",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        20U, 1U, 2U, 200U);
    changes[3] = test_create_change(
        "change-other", "layout.other",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        15U, 0U, 1U, 150U);

    TEST_STATUS_OK(umi_workbench_layout_change_feed_save(
        &feed, &changes[0]));
    TEST_STATUS_OK(umi_workbench_layout_change_feed_save(
        &feed, &changes[1]));
    TEST_STATUS_OK(umi_workbench_layout_change_feed_save(
        &feed, &changes[2]));
    TEST_STATUS_OK(umi_workbench_layout_change_feed_save(
        &feed, &changes[3]));

    TEST_STATUS_OK(umi_workbench_layout_change_feed_list(
        &feed, NULL, NULL, NULL, listed, 8U, &count, &total));
    TEST_REQUIRE(count == 4U && total == 4U,
                 "complete feed listed");
    TEST_REQUIRE(listed[0].sequence == 10U,
                 "feed starts with lowest sequence");
    TEST_REQUIRE(listed[1].sequence == 15U,
                 "second sequence sorted globally");
    TEST_REQUIRE(listed[2].sequence == 20U,
                 "third sequence sorted globally");
    TEST_REQUIRE(listed[3].sequence == 30U,
                 "feed ends with highest sequence");

    TEST_STATUS_OK(umi_workbench_layout_change_feed_list(
        &feed, NULL, change_matches_layout, "layout.sequence",
        listed, 8U, &count, &total));
    TEST_REQUIRE(count == 3U && total == 3U,
                 "layout-specific feed listed");
    TEST_REQUIRE(strcmp(listed[0].layout_id, "layout.sequence") == 0 &&
                 strcmp(listed[2].layout_id, "layout.sequence") == 0,
                 "aggregate filter excludes other layouts");

    umi_data_server_destroy(server);
    return 0;
}

static int test_cursor_monotonicity_and_idempotence(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSyncCursorRepository repository;
    UmiWorkbenchLayoutSyncCursor cursor = make_cursor();
    UmiWorkbenchLayoutSyncCursor loaded;
    uint64_t first_revision;

    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_record_attempt(
        &repository, &cursor, 1000U, "temporary transport failure"));
    TEST_REQUIRE(cursor.last_attempt_at_ms == 1000U,
                 "attempt time retained");
    TEST_REQUIRE(cursor.last_error[0] != '\0',
                 "attempt failure retained");

    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_record_success(
        &repository, &cursor, 20U, 30U, 2000U));
    first_revision = cursor.revision;
    TEST_REQUIRE(cursor.last_pulled_sequence == 20U,
                 "pull sequence advanced");
    TEST_REQUIRE(cursor.last_pushed_sequence == 30U,
                 "push sequence advanced");
    TEST_REQUIRE(cursor.last_error[0] == '\0',
                 "success clears failure evidence");

    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_record_success(
        &repository, &cursor, 10U, 25U, 2500U));
    TEST_REQUIRE(cursor.last_pulled_sequence == 20U,
                 "pull sequence never moves backwards");
    TEST_REQUIRE(cursor.last_pushed_sequence == 30U,
                 "push sequence never moves backwards");
    TEST_REQUIRE(cursor.revision == first_revision + 1U,
                 "idempotent success remains auditable");
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_load(
        &repository, "replica-local", "cursor-sequence", 0U, &loaded));
    TEST_REQUIRE(loaded.last_pulled_sequence == 20U &&
                 loaded.last_pushed_sequence == 30U,
                 "monotonic cursor persisted");

    umi_data_server_destroy(server);
    return 0;
}

static int test_plan_uses_only_unacknowledged_changes(void)
{
    UmiWorkbenchLayoutChange local[3];
    UmiWorkbenchLayoutChange remote[3];
    UmiWorkbenchLayoutSyncCursor cursor = make_cursor();
    UmiWorkbenchLayoutSyncPlanningInput input;
    UmiWorkbenchLayoutSyncPlan plan;

    cursor.last_pushed_sequence = 20U;
    cursor.last_pulled_sequence = 15U;
    local[0] = test_create_change(
        "local-10", "layout.old-local",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        10U, 0U, 1U, 100U);
    local[1] = test_create_change(
        "local-20", "layout.acknowledged-local",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        20U, 1U, 2U, 200U);
    local[2] = test_create_change(
        "local-21", "layout.new-local",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        21U, 0U, 1U, 210U);
    remote[0] = test_create_change(
        "remote-5", "layout.old-remote",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        5U, 0U, 1U, 50U);
    remote[1] = test_create_change(
        "remote-15", "layout.acknowledged-remote",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        15U, 1U, 2U, 150U);
    remote[2] = test_create_change(
        "remote-16", "layout.new-remote",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        16U, 0U, 1U, 160U);

    (void)memset(&input, 0, sizeof(input));
    input.structure_size = sizeof(input);
    input.local_changes = local;
    input.local_change_count = 3U;
    input.remote_changes = remote;
    input.remote_change_count = 3U;
    input.cursor = &cursor;
    input.direction = UMI_WORKBENCH_LAYOUT_DATA_SYNC_BIDIRECTIONAL;
    input.local_replica_id = "replica-local";
    input.remote_replica_id = "replica-remote";
    input.created_at_ms = 5000U;

    TEST_STATUS_OK(umi_workbench_layout_sync_plan_build(&input, &plan));
    TEST_REQUIRE(plan.item_count == 2U,
                 "only unacknowledged changes planned");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_find(
        &plan, "layout.new-local") != NULL,
        "new local change planned");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_find(
        &plan, "layout.new-remote") != NULL,
        "new remote change planned");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_find(
        &plan, "layout.acknowledged-local") == NULL,
        "acknowledged local change excluded");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_find(
        &plan, "layout.acknowledged-remote") == NULL,
        "acknowledged remote change excluded");
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_validate(&plan));
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_feed_order_and_layout_filter() == 0,
                 "feed ordering and filtering");
    TEST_REQUIRE(test_cursor_monotonicity_and_idempotence() == 0,
                 "cursor invariants");
    TEST_REQUIRE(test_plan_uses_only_unacknowledged_changes() == 0,
                 "plan acknowledgement filtering");
    return 0;
}
