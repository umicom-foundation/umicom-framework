/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/sync_engine.c
 *
 * PURPOSE:
 *   Implement explicit plan execution, store-port replication, conflict persistence and cursor advancement for layout synchronisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/sync_engine.h"
#include <stdio.h>
#include <string.h>



/* Initialise result from caller-provided values so later operations receive a known state. */
static void result_init(UmiWorkbenchLayoutSyncResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return;
    (void)memset(result, 0, sizeof(*result));
    result->structure_size = sizeof(*result);
    result->status = UMI_STATUS_OK;
}

/*
 * Provide the workbench layout sync policy default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutSyncPolicy
umi_workbench_layout_sync_policy_default(void)
{
    UmiWorkbenchLayoutSyncPolicy policy;
    (void)memset(&policy, 0, sizeof(policy));
    policy.structure_size = sizeof(policy);
    policy.continue_after_conflict = true;
    policy.allow_remote_delete = false;
    policy.allow_local_delete = false;
    policy.require_fencing_for_push = false;
    policy.maximum_operations = UMI_WORKBENCH_LAYOUT_DATA_MAX_SYNC_ITEMS;
    return policy;
}

/*
 * Initialise workbench layout sync engine from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_sync_engine_init(
    UmiWorkbenchLayoutSyncEngine *engine,
    UmiWorkbenchLayoutConflictStoreRepository *conflicts,
    UmiWorkbenchLayoutSyncCursorRepository *cursors,
    const UmiWorkbenchLayoutSyncPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (engine == NULL || conflicts == NULL || cursors == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(engine, 0, sizeof(*engine));
    engine->structure_size = sizeof(*engine);
    engine->conflicts = conflicts;
    engine->cursors = cursors;
    engine->policy = policy != NULL
        ? *policy : umi_workbench_layout_sync_policy_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (engine->policy.structure_size < sizeof(engine->policy) ||
        engine->policy.maximum_operations == 0U ||
        engine->policy.maximum_operations >
            UMI_WORKBENCH_LAYOUT_DATA_MAX_SYNC_ITEMS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    engine->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout sync engine plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_sync_engine_plan(
    UmiWorkbenchLayoutSyncEngine *engine,
    const UmiWorkbenchLayoutSyncEndpoint *local,
    const UmiWorkbenchLayoutSyncEndpoint *remote,
    const UmiWorkbenchLayoutSyncCursor *cursor,
    UmiWorkbenchLayoutDataSyncDirection direction,
    uint64_t created_at_ms,
    UmiWorkbenchLayoutSyncPlan *out_plan)
{
    UmiWorkbenchLayoutSyncPlanningInput input;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (engine == NULL || local == NULL || remote == NULL ||
        out_plan == NULL ||
        local->structure_size < sizeof(*local) ||
        remote->structure_size < sizeof(*remote)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&input, 0, sizeof(input));
    input.structure_size = sizeof(input);
    input.local_changes = local->changes;
    input.local_change_count = local->change_count;
    input.remote_changes = remote->changes;
    input.remote_change_count = remote->change_count;
    input.cursor = cursor;
    input.direction = direction;
    input.local_replica_id = local->replica_id;
    input.remote_replica_id = remote->replica_id;
    input.created_at_ms = created_at_ms;
    return umi_workbench_layout_sync_plan_build(&input, out_plan);
}

/* Provide the load document operation used by this module and its client applications. */
static UmiStatus load_document(
    UmiWorkbenchLayoutSyncEndpoint *endpoint,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || endpoint->store.load_layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_store_load(
        &endpoint->store, layout_id, out_document);
}

/* Provide the save document operation used by this module and its client applications. */
static UmiStatus save_document(
    UmiWorkbenchLayoutSyncEndpoint *endpoint,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision)
{
    uint64_t resulting_revision = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || endpoint->store.save_layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_store_save(
        &endpoint->store, document, expected_revision,
        &resulting_revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        document->version.revision = resulting_revision;
    }
    return status;
}

/* Provide the delete document operation used by this module and its client applications. */
static UmiStatus delete_document(
    UmiWorkbenchLayoutSyncEndpoint *endpoint,
    const char *layout_id,
    uint64_t expected_revision)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL || endpoint->store.delete_layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_store_delete(
        &endpoint->store, layout_id, expected_revision);
}

/* Provide the copy document operation used by this module and its client applications. */
static UmiStatus copy_document(
    UmiWorkbenchLayoutSyncEndpoint *source,
    UmiWorkbenchLayoutSyncEndpoint *destination,
    const UmiWorkbenchLayoutSyncPlanItem *item,
    bool creating)
{
    UmiWorkbenchLayoutDocument document;
    uint64_t expected_revision = creating ? 0U :
        (destination == source ? item->local_revision :
         item->remote_revision);
    UmiStatus status = load_document(
        source, item->layout_id, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (creating) document.version.revision = 0U;
    return save_document(destination, &document, expected_revision);
}

/* Provide the record conflict operation used by this module and its client applications. */
static UmiStatus record_conflict(
    UmiWorkbenchLayoutSyncEngine *engine,
    const UmiWorkbenchLayoutSyncPlan *plan,
    const UmiWorkbenchLayoutSyncPlanItem *item,
    const char *actor_id,
    uint64_t now_ms)
{
    UmiWorkbenchLayoutConflict conflict;
    (void)memset(&conflict, 0, sizeof(conflict));
    conflict.structure_size = sizeof(conflict);
    (void)snprintf(
        conflict.conflict_id,
        sizeof(conflict.conflict_id),
        "conflict-%016llx-%020llu",
        (unsigned long long)umi_workbench_layout_data_hash_text(
            item->layout_id),
        (unsigned long long)item->sequence);
    (void)umi_workbench_layout_data_copy_text(
        conflict.layout_id, sizeof(conflict.layout_id),
        item->layout_id, false);
    (void)umi_workbench_layout_data_copy_text(
        conflict.local_replica_id,
        sizeof(conflict.local_replica_id),
        plan->local_replica_id, false);
    (void)umi_workbench_layout_data_copy_text(
        conflict.remote_replica_id,
        sizeof(conflict.remote_replica_id),
        plan->remote_replica_id, false);
    conflict.kind =
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE;
    conflict.state = UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN;
    conflict.local_revision = item->local_revision;
    conflict.remote_revision = item->remote_revision;
    conflict.base_revision =
        item->local_revision < item->remote_revision
            ? item->local_revision : item->remote_revision;
    conflict.created_at_ms = now_ms;
    (void)umi_workbench_layout_data_copy_text(
        conflict.message, sizeof(conflict.message),
        item->reason, true);
    (void)actor_id;
    return umi_workbench_layout_conflict_store_save(
        engine->conflicts, &conflict);
}

/* Provide the apply item operation used by this module and its client applications. */
static UmiStatus apply_item(
    UmiWorkbenchLayoutSyncEngine *engine,
    const UmiWorkbenchLayoutSyncPlan *plan,
    const UmiWorkbenchLayoutSyncPlanItem *item,
    UmiWorkbenchLayoutSyncEndpoint *local,
    UmiWorkbenchLayoutSyncEndpoint *remote,
    const char *actor_id,
    uint64_t now_ms)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (item->action) {
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_NO_ACTION:
        return UMI_STATUS_OK;
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_LOCAL:
        return copy_document(remote, local, item, true);
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_LOCAL:
        return copy_document(remote, local, item, false);
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_LOCAL:
        return engine->policy.allow_local_delete
            ? delete_document(local, item->layout_id,
                              item->local_revision)
            : UMI_STATUS_PERMISSION_DENIED;
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE:
        return copy_document(local, remote, item, true);
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE:
        return copy_document(local, remote, item, false);
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE:
        return engine->policy.allow_remote_delete
            ? delete_document(remote, item->layout_id,
                              item->remote_revision)
            : UMI_STATUS_PERMISSION_DENIED;
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT:
        return record_conflict(
            engine, plan, item, actor_id, now_ms);
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
}

/*
 * Perform workbench layout sync engine through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_workbench_layout_sync_engine_apply(
    UmiWorkbenchLayoutSyncEngine *engine,
    const UmiWorkbenchLayoutSyncPlan *plan,
    UmiWorkbenchLayoutSyncEndpoint *local,
    UmiWorkbenchLayoutSyncEndpoint *remote,
    const char *actor_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutSyncResult *out_result)
{
    UmiWorkbenchLayoutSyncResult local_result;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (engine == NULL || plan == NULL || local == NULL ||
        remote == NULL || actor_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    result_init(&local_result);
    local_result.planned_count = plan->item_count;
    status = umi_workbench_layout_sync_plan_validate(plan);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK &&
         index < plan->item_count &&
         index < engine->policy.maximum_operations;
         ++index) {
        const UmiWorkbenchLayoutSyncPlanItem *item =
            &plan->items[index];
        UmiStatus item_status = apply_item(
            engine, plan, item, local, remote,
            actor_id, now_ms);
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->action ==
            UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT) {
            local_result.conflict_count += 1U;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (item_status != UMI_STATUS_OK) {
                local_result.failed_count += 1U;
                status = item_status;
                (void)umi_workbench_layout_data_copy_text(
                    local_result.failed_layout_id,
                    sizeof(local_result.failed_layout_id),
                    item->layout_id, true);
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (item_status != UMI_STATUS_OK ||
                !engine->policy.continue_after_conflict) {
                break;
            }
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->action ==
            UMI_WORKBENCH_LAYOUT_DATA_SYNC_NO_ACTION) {
            local_result.skipped_count += 1U;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (item_status == UMI_STATUS_OK) {
            local_result.applied_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            local_result.failed_count += 1U;
            status = item_status;
            (void)umi_workbench_layout_data_copy_text(
                local_result.failed_layout_id,
                sizeof(local_result.failed_layout_id),
                item->layout_id, true);
            /* Apply this branch only when its contract condition is satisfied. */
            if (!engine->policy.continue_after_conflict) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (item->action >=
                UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_LOCAL &&
            item->action <=
                UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_LOCAL &&
            item->sequence > local_result.last_pulled_sequence) {
            local_result.last_pulled_sequence = item->sequence;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (item->action >=
                UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE &&
            item->action <=
                UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE &&
            item->sequence > local_result.last_pushed_sequence) {
            local_result.last_pushed_sequence = item->sequence;
        }
    }
    local_result.status = status;
    (void)umi_workbench_layout_data_copy_text(
        local_result.message, sizeof(local_result.message),
        status == UMI_STATUS_OK
            ? "Synchronisation plan applied."
            : "Synchronisation stopped after a failed operation.",
        true);
    engine->revision += 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL) *out_result = local_result;
    return status;
}

/*
 * Provide the workbench layout sync engine synchronise operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_sync_engine_synchronise(
    UmiWorkbenchLayoutSyncEngine *engine,
    UmiWorkbenchLayoutSyncEndpoint *local,
    UmiWorkbenchLayoutSyncEndpoint *remote,
    UmiWorkbenchLayoutSyncCursor *cursor,
    UmiWorkbenchLayoutDataSyncDirection direction,
    const char *actor_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutSyncResult *out_result)
{
    UmiWorkbenchLayoutSyncPlan plan;
    UmiWorkbenchLayoutSyncResult result;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_sync_engine_plan(
        engine, local, remote, cursor, direction,
        now_ms, &plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_sync_engine_apply(
            engine, &plan, local, remote, actor_id,
            now_ms, &result);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        result_init(&result);
        result.status = status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_sync_cursor_record_success(
            engine->cursors,
            cursor,
            result.last_pulled_sequence,
            result.last_pushed_sequence,
            now_ms);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)umi_workbench_layout_sync_cursor_record_attempt(
            engine->cursors, cursor, now_ms,
            result.message);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL) *out_result = result;
    return status;
}
