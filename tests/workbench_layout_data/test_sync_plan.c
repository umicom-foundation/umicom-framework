/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_sync_plan.c
 *
 * PURPOSE:
 *   Verify deterministic pull, push and bidirectional layout synchronisation planning with cursor filtering and conflict detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static UmiWorkbenchLayoutSyncCursor empty_cursor(void)
{
    UmiWorkbenchLayoutSyncCursor cursor;
    (void)memset(&cursor, 0, sizeof(cursor));
    cursor.structure_size = sizeof(cursor);
    test_copy_text(cursor.cursor_id, sizeof(cursor.cursor_id), "cursor");
    test_copy_text(cursor.replica_id, sizeof(cursor.replica_id), "local");
    test_copy_text(cursor.peer_id, sizeof(cursor.peer_id), "remote");
    return cursor;
}

static UmiWorkbenchLayoutSyncPlanningInput planning_input(
    const UmiWorkbenchLayoutChange *local,
    size_t local_count,
    const UmiWorkbenchLayoutChange *remote,
    size_t remote_count,
    const UmiWorkbenchLayoutSyncCursor *cursor,
    UmiWorkbenchLayoutDataSyncDirection direction)
{
    UmiWorkbenchLayoutSyncPlanningInput input;
    (void)memset(&input, 0, sizeof(input));
    input.structure_size = sizeof(input);
    input.local_changes = local;
    input.local_change_count = local_count;
    input.remote_changes = remote;
    input.remote_change_count = remote_count;
    input.cursor = cursor;
    input.direction = direction;
    input.local_replica_id = "local";
    input.remote_replica_id = "remote";
    input.created_at_ms = 5000U;
    return input;
}

static int test_push_only_plan(void)
{
    UmiWorkbenchLayoutChange local[2];
    UmiWorkbenchLayoutSyncCursor cursor = empty_cursor();
    UmiWorkbenchLayoutSyncPlanningInput input;
    UmiWorkbenchLayoutSyncPlan plan;
    local[0] = test_create_change(
        "change-create", "layout.new",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        10U, 0U, 1U, 100U);
    local[1] = test_create_change(
        "change-update", "layout.existing",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        20U, 1U, 2U, 200U);
    input = planning_input(local, 2U, NULL, 0U, &cursor,
                           UMI_WORKBENCH_LAYOUT_DATA_SYNC_PUSH);
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_build(&input, &plan));
    TEST_REQUIRE(plan.item_count == 2U, "two push items");
    TEST_REQUIRE(plan.push_count == 2U, "push count");
    TEST_REQUIRE(plan.pull_count == 0U, "no pull items");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_count_action(
        &plan, UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE) == 1U,
        "create remote planned");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_count_action(
        &plan, UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE) == 1U,
        "update remote planned");
    TEST_REQUIRE(plan.items[0].sequence == 10U,
                 "plan sorted by sequence");
    return 0;
}

static int test_pull_only_plan(void)
{
    UmiWorkbenchLayoutChange remote[2];
    UmiWorkbenchLayoutSyncCursor cursor = empty_cursor();
    UmiWorkbenchLayoutSyncPlanningInput input;
    UmiWorkbenchLayoutSyncPlan plan;
    remote[0] = test_create_change(
        "remote-create", "layout.remote",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        11U, 0U, 1U, 110U);
    remote[1] = test_create_change(
        "remote-delete", "layout.removed",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED,
        22U, 3U, 4U, 0U);
    input = planning_input(NULL, 0U, remote, 2U, &cursor,
                           UMI_WORKBENCH_LAYOUT_DATA_SYNC_PULL);
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_build(&input, &plan));
    TEST_REQUIRE(plan.pull_count == 2U, "pull count");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_count_action(
        &plan, UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_LOCAL) == 1U,
        "create local planned");
    TEST_REQUIRE(umi_workbench_layout_sync_plan_count_action(
        &plan, UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_LOCAL) == 1U,
        "delete local planned");
    return 0;
}

static int test_compatible_lineage(void)
{
    UmiWorkbenchLayoutChange local;
    UmiWorkbenchLayoutChange remote;
    UmiWorkbenchLayoutSyncCursor cursor = empty_cursor();
    UmiWorkbenchLayoutSyncPlanningInput input;
    UmiWorkbenchLayoutSyncPlan plan;
    local = test_create_change(
        "local-change", "layout.shared",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        10U, 1U, 2U, 200U);
    remote = test_create_change(
        "remote-change", "layout.shared",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        20U, 2U, 3U, 300U);
    input = planning_input(&local, 1U, &remote, 1U, &cursor,
                           UMI_WORKBENCH_LAYOUT_DATA_SYNC_BIDIRECTIONAL);
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_build(&input, &plan));
    TEST_REQUIRE(plan.item_count == 1U, "one shared item");
    TEST_REQUIRE(plan.items[0].action ==
        UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_LOCAL,
        "newer remote lineage pulled");
    TEST_REQUIRE(plan.conflict_count == 0U,
                 "compatible lineage has no conflict");
    return 0;
}

static int test_conflicting_lineage(void)
{
    UmiWorkbenchLayoutChange local;
    UmiWorkbenchLayoutChange remote;
    UmiWorkbenchLayoutSyncCursor cursor = empty_cursor();
    UmiWorkbenchLayoutSyncPlanningInput input;
    UmiWorkbenchLayoutSyncPlan plan;
    local = test_create_change(
        "local-change", "layout.shared",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        10U, 1U, 2U, 200U);
    remote = test_create_change(
        "remote-change", "layout.shared",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        20U, 7U, 8U, 800U);
    input = planning_input(&local, 1U, &remote, 1U, &cursor,
                           UMI_WORKBENCH_LAYOUT_DATA_SYNC_BIDIRECTIONAL);
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_build(&input, &plan));
    TEST_REQUIRE(plan.item_count == 1U, "one conflict item");
    TEST_REQUIRE(plan.items[0].action ==
        UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT,
        "conflict planned");
    TEST_REQUIRE(plan.conflict_count == 1U,
                 "conflict count updated");
    TEST_REQUIRE(strstr(plan.items[0].reason, "incompatible") != NULL,
                 "conflict evidence retained");
    return 0;
}

static int test_cursor_filtering(void)
{
    UmiWorkbenchLayoutChange local[2];
    UmiWorkbenchLayoutSyncCursor cursor = empty_cursor();
    UmiWorkbenchLayoutSyncPlanningInput input;
    UmiWorkbenchLayoutSyncPlan plan;
    cursor.last_pushed_sequence = 10U;
    local[0] = test_create_change(
        "already-pushed", "layout.old",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        10U, 0U, 1U, 100U);
    local[1] = test_create_change(
        "new-change", "layout.new",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        11U, 0U, 1U, 110U);
    input = planning_input(local, 2U, NULL, 0U, &cursor,
                           UMI_WORKBENCH_LAYOUT_DATA_SYNC_PUSH);
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_build(&input, &plan));
    TEST_REQUIRE(plan.item_count == 1U,
                 "cursor removes acknowledged change");
    TEST_REQUIRE(strcmp(plan.items[0].layout_id, "layout.new") == 0,
                 "new change remains");
    return 0;
}

static int test_duplicate_item_rejected(void)
{
    UmiWorkbenchLayoutSyncPlan plan;
    UmiWorkbenchLayoutSyncPlanItem item;
    (void)memset(&plan, 0, sizeof(plan));
    plan.structure_size = sizeof(plan);
    test_copy_text(plan.plan_id, sizeof(plan.plan_id), "plan");
    test_copy_text(plan.local_replica_id,
                   sizeof(plan.local_replica_id), "local");
    test_copy_text(plan.remote_replica_id,
                   sizeof(plan.remote_replica_id), "remote");
    (void)memset(&item, 0, sizeof(item));
    item.structure_size = sizeof(item);
    test_copy_text(item.layout_id, sizeof(item.layout_id), "layout.same");
    item.action = UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE;
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_add_item(&plan, &item));
    TEST_REQUIRE(umi_workbench_layout_sync_plan_add_item(&plan, &item) ==
        UMI_STATUS_ALREADY_EXISTS, "duplicate layout rejected");
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_validate(&plan));
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_push_only_plan() == 0, "push plan");
    TEST_REQUIRE(test_pull_only_plan() == 0, "pull plan");
    TEST_REQUIRE(test_compatible_lineage() == 0,
                 "compatible lineage");
    TEST_REQUIRE(test_conflicting_lineage() == 0,
                 "conflicting lineage");
    TEST_REQUIRE(test_cursor_filtering() == 0,
                 "cursor filtering");
    TEST_REQUIRE(test_duplicate_item_rejected() == 0,
                 "duplicate item");
    return 0;
}
