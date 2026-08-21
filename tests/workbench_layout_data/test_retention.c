/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_retention.c
 *
 * PURPOSE:
 *   Verify policy-driven pruning of delivered, resolved, completed and expired transient records while preserving authoritative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int seed_retention_records(
    UmiDataServer *server,
    UmiWorkbenchLayoutOutboxRepository *outbox,
    UmiWorkbenchLayoutConflictStoreRepository *conflicts,
    UmiWorkbenchLayoutOfflineQueueRepository *offline,
    UmiWorkbenchLayoutPresenceStoreRepository *presence)
{
    UmiWorkbenchLayoutOutboxEntry outbox_entry;
    (void)server;
    UmiWorkbenchLayoutConflict conflict;
    UmiWorkbenchLayoutOfflineOperation operation;
    UmiWorkbenchLayoutPresence presence_record;
    (void)memset(&outbox_entry, 0, sizeof(outbox_entry));
    outbox_entry.structure_size = sizeof(outbox_entry);
    test_copy_text(outbox_entry.entry_id,
                   sizeof(outbox_entry.entry_id), "outbox-old");
    test_copy_text(outbox_entry.topic,
                   sizeof(outbox_entry.topic), "layout.changed");
    test_copy_text(outbox_entry.aggregate_id,
                   sizeof(outbox_entry.aggregate_id), "layout.retention");
    outbox_entry.state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DELIVERED;
    outbox_entry.sequence = 1U;
    outbox_entry.created_at_ms = 100U;
    outbox_entry.leased_until_ms = 200U;
    outbox_entry.maximum_attempts = 3U;
    TEST_STATUS_OK(umi_workbench_layout_outbox_save(
        outbox, &outbox_entry));

    (void)memset(&conflict, 0, sizeof(conflict));
    conflict.structure_size = sizeof(conflict);
    test_copy_text(conflict.conflict_id,
                   sizeof(conflict.conflict_id), "conflict-old");
    test_copy_text(conflict.layout_id,
                   sizeof(conflict.layout_id), "layout.retention");
    test_copy_text(conflict.local_replica_id,
                   sizeof(conflict.local_replica_id), "local");
    test_copy_text(conflict.remote_replica_id,
                   sizeof(conflict.remote_replica_id), "remote");
    conflict.kind =
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE;
    conflict.state =
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_MERGED;
    conflict.created_at_ms = 100U;
    conflict.resolved_at_ms = 200U;
    test_copy_text(conflict.resolved_by,
                   sizeof(conflict.resolved_by), "developer");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_save(
        conflicts, &conflict));

    (void)memset(&operation, 0, sizeof(operation));
    operation.structure_size = sizeof(operation);
    test_copy_text(operation.operation_id,
                   sizeof(operation.operation_id), "operation-old");
    test_copy_text(operation.layout_id,
                   sizeof(operation.layout_id), "layout.retention");
    test_copy_text(operation.actor_id,
                   sizeof(operation.actor_id), "developer");
    operation.change_kind = UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED;
    operation.state = UMI_WORKBENCH_LAYOUT_DATA_OPERATION_COMPLETED;
    operation.created_at_ms = 100U;
    operation.available_at_ms = 200U;
    operation.maximum_attempts = 3U;
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_save(
        offline, &operation));

    (void)memset(&presence_record, 0, sizeof(presence_record));
    presence_record.structure_size = sizeof(presence_record);
    test_copy_text(presence_record.presence_id,
                   sizeof(presence_record.presence_id), "presence-old");
    test_copy_text(presence_record.layout_id,
                   sizeof(presence_record.layout_id), "layout.retention");
    test_copy_text(presence_record.user_id,
                   sizeof(presence_record.user_id), "developer");
    test_copy_text(presence_record.client_id,
                   sizeof(presence_record.client_id), "client-a");
    presence_record.connected_at_ms = 100U;
    presence_record.last_seen_at_ms = 100U;
    presence_record.expires_at_ms = 200U;
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        presence, &presence_record));
    return 0;
}

static int test_dry_run(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOutboxRepository outbox;
    UmiWorkbenchLayoutConflictStoreRepository conflicts;
    UmiWorkbenchLayoutOfflineQueueRepository offline;
    UmiWorkbenchLayoutPresenceStoreRepository presence;
    UmiWorkbenchLayoutRetentionPolicy policy =
        umi_workbench_layout_retention_policy_default();
    UmiWorkbenchLayoutRetentionResult result;
    size_t before;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_outbox_repository_init(
        &outbox, server));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_repository_init(
        &offline, server));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_repository_init(
        &presence, server));
    TEST_REQUIRE(seed_retention_records(
        server, &outbox, &conflicts, &offline, &presence) == 0,
        "seed records");
    before = umi_data_server_count(server);
    policy.delivered_outbox_retention_ms = 0U;
    policy.resolved_conflict_retention_ms = 0U;
    policy.completed_operation_retention_ms = 0U;
    policy.expired_presence_retention_ms = 0U;
    policy.dry_run = true;
    TEST_STATUS_OK(umi_workbench_layout_retention_apply(
        server, &policy, 1000U, &result));
    TEST_REQUIRE(result.eligible_count == 4U,
                 "four records eligible");
    TEST_REQUIRE(result.deleted_count == 0U,
                 "dry run deletes nothing");
    TEST_REQUIRE(umi_data_server_count(server) == before,
                 "dry run preserves data");
    umi_data_server_destroy(server);
    return 0;
}

static int test_pruning(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOutboxRepository outbox;
    UmiWorkbenchLayoutConflictStoreRepository conflicts;
    UmiWorkbenchLayoutOfflineQueueRepository offline;
    UmiWorkbenchLayoutPresenceStoreRepository presence;
    UmiWorkbenchLayoutRetentionPolicy policy =
        umi_workbench_layout_retention_policy_default();
    UmiWorkbenchLayoutRetentionResult result;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_outbox_repository_init(
        &outbox, server));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_repository_init(
        &offline, server));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_repository_init(
        &presence, server));
    TEST_REQUIRE(seed_retention_records(
        server, &outbox, &conflicts, &offline, &presence) == 0,
        "seed records");
    policy.delivered_outbox_retention_ms = 0U;
    policy.resolved_conflict_retention_ms = 0U;
    policy.completed_operation_retention_ms = 0U;
    policy.expired_presence_retention_ms = 0U;
    policy.dry_run = false;
    TEST_STATUS_OK(umi_workbench_layout_retention_apply(
        server, &policy, 1000U, &result));
    TEST_REQUIRE(result.deleted_count == 4U,
                 "eligible transient records pruned");
    TEST_REQUIRE(result.status == UMI_STATUS_OK,
                 "retention succeeded");
    umi_data_server_destroy(server);
    return 0;
}

static int test_authoritative_layout_preserved(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutRetentionPolicy policy =
        umi_workbench_layout_retention_policy_default();
    UmiWorkbenchLayoutRetentionResult result;
    char value[64];
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/layout-manifest/layout.keep",
        "authoritative"));
    policy.delivered_outbox_retention_ms = 0U;
    policy.resolved_conflict_retention_ms = 0U;
    policy.completed_operation_retention_ms = 0U;
    policy.expired_presence_retention_ms = 0U;
    TEST_STATUS_OK(umi_workbench_layout_retention_apply(
        server, &policy, 1000U, &result));
    TEST_STATUS_OK(umi_data_server_get(
        server, "workbench-layout/layout-manifest/layout.keep",
        value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "authoritative") == 0,
                 "layout manifest preserved");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_dry_run() == 0, "retention dry run");
    TEST_REQUIRE(test_pruning() == 0, "retention pruning");
    TEST_REQUIRE(test_authoritative_layout_preserved() == 0,
                 "authoritative preservation");
    return 0;
}
