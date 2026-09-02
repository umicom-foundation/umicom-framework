/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_sync_policy.c
 *
 * PURPOSE:
 *   Verify synchronisation policy defaults, operation budgets, conflict continuation and delete permissions without hidden mutation.
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
 * Exercise test default policy and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_default_policy(void)
{
    UmiWorkbenchLayoutSyncPolicy policy =
        umi_workbench_layout_sync_policy_default();
    TEST_REQUIRE(policy.structure_size == sizeof(policy),
                 "policy structure size");
    TEST_REQUIRE(policy.continue_after_conflict,
                 "default continues after recorded conflicts");
    TEST_REQUIRE(!policy.allow_remote_delete,
                 "remote delete disabled by default");
    TEST_REQUIRE(!policy.allow_local_delete,
                 "local delete disabled by default");
    TEST_REQUIRE(policy.maximum_operations > 0U,
                 "operation budget configured");
    return 0;
}

/*
 * Exercise test operation budget and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_operation_budget(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutConflictStoreRepository conflicts;
    UmiWorkbenchLayoutSyncCursorRepository cursors;
    UmiWorkbenchLayoutSyncPolicy policy =
        umi_workbench_layout_sync_policy_default();
    UmiWorkbenchLayoutSyncEngine engine;
    UmiWorkbenchLayoutSyncPlan plan;
    UmiWorkbenchLayoutSyncPlanItem item;
    UmiWorkbenchMemoryStore local_store;
    UmiWorkbenchMemoryStore remote_store;
    UmiWorkbenchLayoutSyncEndpoint local;
    UmiWorkbenchLayoutSyncEndpoint remote;
    UmiWorkbenchLayoutSyncResult result;
    size_t index;
    TEST_REQUIRE(server != NULL, "create server");
    policy.maximum_operations = 2U;
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &cursors, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_init(
        &engine, &conflicts, &cursors, &policy));
    umi_workbench_memory_store_init(&local_store);
    umi_workbench_memory_store_init(&remote_store);
    (void)memset(&local, 0, sizeof(local));
    local.structure_size = sizeof(local);
    test_copy_text(local.replica_id, sizeof(local.replica_id), "local");
    local.store = umi_workbench_memory_store_adapter(&local_store);
    (void)memset(&remote, 0, sizeof(remote));
    remote.structure_size = sizeof(remote);
    test_copy_text(remote.replica_id, sizeof(remote.replica_id), "remote");
    remote.store = umi_workbench_memory_store_adapter(&remote_store);
    (void)memset(&plan, 0, sizeof(plan));
    plan.structure_size = sizeof(plan);
    test_copy_text(plan.plan_id, sizeof(plan.plan_id), "budget-plan");
    test_copy_text(plan.local_replica_id,
                   sizeof(plan.local_replica_id), "local");
    test_copy_text(plan.remote_replica_id,
                   sizeof(plan.remote_replica_id), "remote");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 4U; ++index) {
        (void)memset(&item, 0, sizeof(item));
        item.structure_size = sizeof(item);
        (void)snprintf(item.layout_id,
                       sizeof(item.layout_id), "layout-%zu", index);
        item.action = UMI_WORKBENCH_LAYOUT_DATA_SYNC_NO_ACTION;
        item.sequence = index + 1U;
        TEST_STATUS_OK(umi_workbench_layout_sync_plan_add_item(
            &plan, &item));
    }
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_apply(
        &engine, &plan, &local, &remote,
        "developer", 1000U, &result));
    TEST_REQUIRE(result.planned_count == 4U,
                 "complete plan count retained");
    TEST_REQUIRE(result.skipped_count == 2U,
                 "operation budget limits execution");
    umi_workbench_memory_store_clear(&local_store);
    umi_workbench_memory_store_clear(&remote_store);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test conflict stop policy and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_conflict_stop_policy(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutConflictStoreRepository conflicts;
    UmiWorkbenchLayoutSyncCursorRepository cursors;
    UmiWorkbenchLayoutSyncPolicy policy =
        umi_workbench_layout_sync_policy_default();
    UmiWorkbenchLayoutSyncEngine engine;
    UmiWorkbenchLayoutSyncPlan plan;
    UmiWorkbenchLayoutSyncPlanItem item;
    UmiWorkbenchMemoryStore local_store;
    UmiWorkbenchMemoryStore remote_store;
    UmiWorkbenchLayoutSyncEndpoint local;
    UmiWorkbenchLayoutSyncEndpoint remote;
    UmiWorkbenchLayoutSyncResult result;
    TEST_REQUIRE(server != NULL, "create server");
    policy.continue_after_conflict = false;
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &cursors, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_init(
        &engine, &conflicts, &cursors, &policy));
    umi_workbench_memory_store_init(&local_store);
    umi_workbench_memory_store_init(&remote_store);
    (void)memset(&local, 0, sizeof(local));
    local.structure_size = sizeof(local);
    test_copy_text(local.replica_id, sizeof(local.replica_id), "local");
    local.store = umi_workbench_memory_store_adapter(&local_store);
    (void)memset(&remote, 0, sizeof(remote));
    remote.structure_size = sizeof(remote);
    test_copy_text(remote.replica_id, sizeof(remote.replica_id), "remote");
    remote.store = umi_workbench_memory_store_adapter(&remote_store);
    (void)memset(&plan, 0, sizeof(plan));
    plan.structure_size = sizeof(plan);
    test_copy_text(plan.plan_id, sizeof(plan.plan_id), "conflict-plan");
    test_copy_text(plan.local_replica_id,
                   sizeof(plan.local_replica_id), "local");
    test_copy_text(plan.remote_replica_id,
                   sizeof(plan.remote_replica_id), "remote");
    (void)memset(&item, 0, sizeof(item));
    item.structure_size = sizeof(item);
    test_copy_text(item.layout_id,
                   sizeof(item.layout_id), "layout.conflict");
    test_copy_text(item.reason,
                   sizeof(item.reason), "incompatible revisions");
    item.action = UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT;
    item.sequence = 1U;
    item.local_revision = 2U;
    item.remote_revision = 3U;
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_add_item(&plan, &item));
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_apply(
        &engine, &plan, &local, &remote,
        "developer", 1000U, &result));
    TEST_REQUIRE(result.conflict_count == 1U,
                 "conflict recorded");
    TEST_REQUIRE(result.applied_count == 0U,
                 "conflict not counted as normal apply");
    umi_workbench_memory_store_clear(&local_store);
    umi_workbench_memory_store_clear(&remote_store);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_default_policy() == 0, "default policy");
    TEST_REQUIRE(test_operation_budget() == 0,
                 "operation budget");
    TEST_REQUIRE(test_conflict_stop_policy() == 0,
                 "conflict stop policy");
    return 0;
}
