/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_sync_engine.c
 *
 * PURPOSE:
 *   Verify execution of deterministic synchronisation plans across memory-backed replicas with cursor and conflict evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static void endpoint_init(
    UmiWorkbenchLayoutSyncEndpoint *endpoint,
    const char *replica_id,
    UmiWorkbenchMemoryStore *store,
    const UmiWorkbenchLayoutChange *changes,
    size_t change_count)
{
    (void)memset(endpoint, 0, sizeof(*endpoint));
    endpoint->structure_size = sizeof(*endpoint);
    test_copy_text(endpoint->replica_id,
                   sizeof(endpoint->replica_id), replica_id);
    endpoint->store = umi_workbench_memory_store_adapter(store);
    endpoint->changes = changes;
    endpoint->change_count = change_count;
}

static int test_create_remote(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutConflictStoreRepository conflicts;
    UmiWorkbenchLayoutSyncCursorRepository cursors;
    UmiWorkbenchLayoutSyncEngine engine;
    UmiWorkbenchMemoryStore local_store;
    UmiWorkbenchMemoryStore remote_store;
    UmiWorkbenchLayoutSyncEndpoint local;
    UmiWorkbenchLayoutSyncEndpoint remote;
    UmiWorkbenchLayoutChange change = test_create_change(
        "change-create", "layout.sync",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED,
        1U, 0U, 1U, 100U);
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.sync");
    UmiWorkbenchLayoutDocument loaded;
    UmiWorkbenchLayoutSyncCursor cursor;
    UmiWorkbenchLayoutSyncResult result;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &cursors, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_init(
        &engine, &conflicts, &cursors, NULL));
    umi_workbench_memory_store_init(&local_store);
    umi_workbench_memory_store_init(&remote_store);
    {
        UmiWorkbenchLayoutStoreAdapter local_adapter =
            umi_workbench_memory_store_adapter(&local_store);
        TEST_STATUS_OK(umi_workbench_layout_store_save(
            &local_adapter, document, 0U, &revision));
    }
    endpoint_init(&local, "local", &local_store, &change, 1U);
    endpoint_init(&remote, "remote", &remote_store, NULL, 0U);
    (void)memset(&cursor, 0, sizeof(cursor));
    cursor.structure_size = sizeof(cursor);
    test_copy_text(cursor.cursor_id, sizeof(cursor.cursor_id), "cursor");
    test_copy_text(cursor.replica_id, sizeof(cursor.replica_id), "local");
    test_copy_text(cursor.peer_id, sizeof(cursor.peer_id), "remote");
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_synchronise(
        &engine, &local, &remote, &cursor,
        UMI_WORKBENCH_LAYOUT_DATA_SYNC_PUSH,
        "developer", 2000U, &result));
    TEST_REQUIRE(result.applied_count == 1U,
                 "one change applied");
    TEST_STATUS_OK(umi_workbench_layout_store_load(
        &remote.store, "layout.sync", &loaded));
    TEST_REQUIRE(strcmp(loaded.identity.layout_id, "layout.sync") == 0,
                 "remote layout created");
    TEST_REQUIRE(cursor.last_pushed_sequence == 1U,
                 "push cursor advanced");
    umi_workbench_memory_store_clear(&local_store);
    umi_workbench_memory_store_clear(&remote_store);
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

static int test_conflict_recording(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutConflictStoreRepository conflicts;
    UmiWorkbenchLayoutSyncCursorRepository cursors;
    UmiWorkbenchLayoutSyncEngine engine;
    UmiWorkbenchMemoryStore local_store;
    UmiWorkbenchMemoryStore remote_store;
    UmiWorkbenchLayoutSyncEndpoint local;
    UmiWorkbenchLayoutSyncEndpoint remote;
    UmiWorkbenchLayoutChange local_change = test_create_change(
        "local-change", "layout.conflict",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        10U, 1U, 2U, 200U);
    UmiWorkbenchLayoutChange remote_change = test_create_change(
        "remote-change", "layout.conflict",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        11U, 7U, 8U, 800U);
    UmiWorkbenchLayoutSyncCursor cursor;
    UmiWorkbenchLayoutSyncResult result;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &cursors, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_init(
        &engine, &conflicts, &cursors, NULL));
    umi_workbench_memory_store_init(&local_store);
    umi_workbench_memory_store_init(&remote_store);
    endpoint_init(&local, "local", &local_store, &local_change, 1U);
    endpoint_init(&remote, "remote", &remote_store, &remote_change, 1U);
    (void)memset(&cursor, 0, sizeof(cursor));
    cursor.structure_size = sizeof(cursor);
    test_copy_text(cursor.cursor_id, sizeof(cursor.cursor_id), "cursor");
    test_copy_text(cursor.replica_id, sizeof(cursor.replica_id), "local");
    test_copy_text(cursor.peer_id, sizeof(cursor.peer_id), "remote");
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_synchronise(
        &engine, &local, &remote, &cursor,
        UMI_WORKBENCH_LAYOUT_DATA_SYNC_BIDIRECTIONAL,
        "developer", 2000U, &result));
    TEST_REQUIRE(result.conflict_count == 1U,
                 "conflict result counted");
    TEST_REQUIRE(umi_workbench_layout_conflict_store_open_count(
        &conflicts) == 1U, "conflict persisted");
    umi_workbench_memory_store_clear(&local_store);
    umi_workbench_memory_store_clear(&remote_store);
    umi_data_server_destroy(server);
    return 0;
}

static int test_delete_policy(void)
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
    policy.allow_remote_delete = false;
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_cursor_repository_init(
        &cursors, server));
    TEST_STATUS_OK(umi_workbench_layout_sync_engine_init(
        &engine, &conflicts, &cursors, &policy));
    umi_workbench_memory_store_init(&local_store);
    umi_workbench_memory_store_init(&remote_store);
    endpoint_init(&local, "local", &local_store, NULL, 0U);
    endpoint_init(&remote, "remote", &remote_store, NULL, 0U);
    (void)memset(&plan, 0, sizeof(plan));
    plan.structure_size = sizeof(plan);
    test_copy_text(plan.plan_id, sizeof(plan.plan_id), "delete-plan");
    test_copy_text(plan.local_replica_id,
                   sizeof(plan.local_replica_id), "local");
    test_copy_text(plan.remote_replica_id,
                   sizeof(plan.remote_replica_id), "remote");
    (void)memset(&item, 0, sizeof(item));
    item.structure_size = sizeof(item);
    test_copy_text(item.layout_id,
                   sizeof(item.layout_id), "layout.delete");
    item.action = UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE;
    item.remote_revision = 1U;
    TEST_STATUS_OK(umi_workbench_layout_sync_plan_add_item(&plan, &item));
    TEST_REQUIRE(umi_workbench_layout_sync_engine_apply(
        &engine, &plan, &local, &remote,
        "developer", 2000U, &result) ==
        UMI_STATUS_PERMISSION_DENIED,
        "remote delete denied by policy");
    TEST_REQUIRE(result.failed_count == 1U,
                 "failed delete counted");
    umi_workbench_memory_store_clear(&local_store);
    umi_workbench_memory_store_clear(&remote_store);
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_create_remote() == 0, "create remote");
    TEST_REQUIRE(test_conflict_recording() == 0,
                 "conflict recording");
    TEST_REQUIRE(test_delete_policy() == 0, "delete policy");
    return 0;
}
