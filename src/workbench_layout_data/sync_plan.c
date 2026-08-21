/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/sync_plan.c
 *
 * PURPOSE:
 *   Implement deterministic change-feed comparison, conflict identification, direction policy and stable synchronisation ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/sync_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static const UmiWorkbenchLayoutChange *latest_change(
    const UmiWorkbenchLayoutChange *changes,
    size_t count,
    const char *layout_id,
    uint64_t minimum_sequence)
{
    const UmiWorkbenchLayoutChange *latest = NULL;
    size_t index;
    for (index = 0U; index < count; ++index) {
        if (changes[index].sequence <= minimum_sequence ||
            strcmp(changes[index].layout_id, layout_id) != 0) {
            continue;
        }
        if (latest == NULL ||
            changes[index].sequence > latest->sequence) {
            latest = &changes[index];
        }
    }
    return latest;
}

static bool plan_has_layout(
    const UmiWorkbenchLayoutSyncPlan *plan,
    const char *layout_id)
{
    return umi_workbench_layout_sync_plan_find(
        plan, layout_id) != NULL;
}

static UmiWorkbenchLayoutDataSyncAction pull_action(
    const UmiWorkbenchLayoutChange *change)
{
    return change->kind == UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED
        ? UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_LOCAL
        : change->base_revision == 0U
            ? UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_LOCAL
            : UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_LOCAL;
}

static UmiWorkbenchLayoutDataSyncAction push_action(
    const UmiWorkbenchLayoutChange *change)
{
    return change->kind == UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED
        ? UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE
        : change->base_revision == 0U
            ? UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE
            : UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE;
}

static bool changes_compatible(
    const UmiWorkbenchLayoutChange *local,
    const UmiWorkbenchLayoutChange *remote)
{
    if (local == NULL || remote == NULL) return true;
    if (local->content_hash == remote->content_hash &&
        local->kind == remote->kind) return true;
    if (local->revision == remote->base_revision ||
        remote->revision == local->base_revision) return true;
    return false;
}

static void fill_item(
    UmiWorkbenchLayoutSyncPlanItem *item,
    const UmiWorkbenchLayoutChange *change,
    UmiWorkbenchLayoutDataSyncAction action,
    uint64_t local_revision,
    uint64_t remote_revision,
    const char *reason)
{
    (void)memset(item, 0, sizeof(*item));
    item->structure_size = sizeof(*item);
    item->action = action;
    item->local_revision = local_revision;
    item->remote_revision = remote_revision;
    item->sequence = change != NULL ? change->sequence : 0U;
    item->content_hash = change != NULL ? change->content_hash : 0U;
    item->requires_lease =
        action == UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE ||
        action == UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE;
    item->requires_permission_check =
        action != UMI_WORKBENCH_LAYOUT_DATA_SYNC_NO_ACTION;
    if (change != NULL) {
        (void)umi_workbench_layout_data_copy_text(
            item->layout_id, sizeof(item->layout_id),
            change->layout_id, false);
        (void)umi_workbench_layout_data_copy_text(
            item->change_id, sizeof(item->change_id),
            change->change_id, true);
    }
    (void)umi_workbench_layout_data_copy_text(
        item->reason, sizeof(item->reason),
        reason != NULL ? reason : "", true);
}

UmiStatus umi_workbench_layout_sync_plan_add_item(
    UmiWorkbenchLayoutSyncPlan *plan,
    const UmiWorkbenchLayoutSyncPlanItem *item)
{
    if (plan == NULL || item == NULL ||
        item->structure_size < sizeof(*item) ||
        item->layout_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->item_count >= UMI_WORKBENCH_LAYOUT_DATA_MAX_SYNC_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (plan_has_layout(plan, item->layout_id)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    plan->items[plan->item_count++] = *item;
    switch (item->action) {
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_LOCAL:
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_LOCAL:
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_LOCAL:
        plan->pull_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE:
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE:
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE:
        plan->push_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT:
        plan->conflict_count += 1U;
        break;
    default:
        break;
    }
    return UMI_STATUS_OK;
}

static UmiStatus consider_local_change(
    const UmiWorkbenchLayoutSyncPlanningInput *input,
    const UmiWorkbenchLayoutChange *local,
    UmiWorkbenchLayoutSyncPlan *plan)
{
    const UmiWorkbenchLayoutChange *remote =
        latest_change(input->remote_changes,
                      input->remote_change_count,
                      local->layout_id,
                      input->cursor != NULL
                          ? input->cursor->last_pulled_sequence : 0U);
    UmiWorkbenchLayoutSyncPlanItem item;
    if (plan_has_layout(plan, local->layout_id)) {
        return UMI_STATUS_OK;
    }
    if (remote == NULL) {
        if (input->direction == UMI_WORKBENCH_LAYOUT_DATA_SYNC_PULL) {
            return UMI_STATUS_OK;
        }
        fill_item(&item, local, push_action(local),
                  local->revision, 0U,
                  "The local replica has an unapplied change.");
        return umi_workbench_layout_sync_plan_add_item(plan, &item);
    }
    if (changes_compatible(local, remote)) {
        const UmiWorkbenchLayoutChange *winner =
            local->sequence >= remote->sequence ? local : remote;
        UmiWorkbenchLayoutDataSyncAction action;
        if (local->content_hash == remote->content_hash &&
            local->kind == remote->kind) {
            action = UMI_WORKBENCH_LAYOUT_DATA_SYNC_NO_ACTION;
        } else if (winner == local) {
            action = push_action(local);
        } else {
            action = pull_action(remote);
        }
        fill_item(&item, winner, action,
                  local->revision, remote->revision,
                  "Compatible change lineage selected the newest evidence.");
        return umi_workbench_layout_sync_plan_add_item(plan, &item);
    }
    fill_item(&item, local,
              UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT,
              local->revision, remote->revision,
              "Both replicas changed the same layout from incompatible bases.");
    return umi_workbench_layout_sync_plan_add_item(plan, &item);
}

static UmiStatus consider_remote_change(
    const UmiWorkbenchLayoutSyncPlanningInput *input,
    const UmiWorkbenchLayoutChange *remote,
    UmiWorkbenchLayoutSyncPlan *plan)
{
    UmiWorkbenchLayoutSyncPlanItem item;
    if (plan_has_layout(plan, remote->layout_id)) {
        return UMI_STATUS_OK;
    }
    if (input->direction == UMI_WORKBENCH_LAYOUT_DATA_SYNC_PUSH) {
        return UMI_STATUS_OK;
    }
    fill_item(&item, remote, pull_action(remote),
              0U, remote->revision,
              "The remote replica has an unapplied change.");
    return umi_workbench_layout_sync_plan_add_item(plan, &item);
}

UmiStatus umi_workbench_layout_sync_plan_build(
    const UmiWorkbenchLayoutSyncPlanningInput *input,
    UmiWorkbenchLayoutSyncPlan *out_plan)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (input == NULL || out_plan == NULL ||
        input->structure_size < sizeof(*input) ||
        input->local_replica_id == NULL ||
        input->remote_replica_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->structure_size = sizeof(*out_plan);
    out_plan->direction = input->direction;
    out_plan->created_at_ms = input->created_at_ms;
    out_plan->from_sequence = input->cursor != NULL
        ? input->cursor->last_pulled_sequence : 0U;
    (void)umi_workbench_layout_data_copy_text(
        out_plan->local_replica_id,
        sizeof(out_plan->local_replica_id),
        input->local_replica_id, false);
    (void)umi_workbench_layout_data_copy_text(
        out_plan->remote_replica_id,
        sizeof(out_plan->remote_replica_id),
        input->remote_replica_id, false);
    (void)snprintf(out_plan->plan_id, sizeof(out_plan->plan_id),
                   "sync-%s-%s-%020llu",
                   input->local_replica_id,
                   input->remote_replica_id,
                   (unsigned long long)input->created_at_ms);

    for (index = 0U;
         status == UMI_STATUS_OK &&
         index < input->local_change_count;
         ++index) {
        const UmiWorkbenchLayoutChange *change =
            &input->local_changes[index];
        if (input->cursor != NULL &&
            change->sequence <= input->cursor->last_pushed_sequence) {
            continue;
        }
        status = consider_local_change(input, change, out_plan);
        if (change->sequence > out_plan->to_sequence) {
            out_plan->to_sequence = change->sequence;
        }
    }
    for (index = 0U;
         status == UMI_STATUS_OK &&
         index < input->remote_change_count;
         ++index) {
        const UmiWorkbenchLayoutChange *change =
            &input->remote_changes[index];
        if (input->cursor != NULL &&
            change->sequence <= input->cursor->last_pulled_sequence) {
            continue;
        }
        status = consider_remote_change(input, change, out_plan);
        if (change->sequence > out_plan->to_sequence) {
            out_plan->to_sequence = change->sequence;
        }
    }
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_sync_plan_sort(out_plan);
        status = umi_workbench_layout_sync_plan_validate(out_plan);
    }
    return status;
}

UmiStatus umi_workbench_layout_sync_plan_validate(
    const UmiWorkbenchLayoutSyncPlan *plan)
{
    size_t index;
    size_t nested;
    if (plan == NULL || plan->structure_size < sizeof(*plan) ||
        plan->plan_id[0] == '\0' ||
        plan->local_replica_id[0] == '\0' ||
        plan->remote_replica_id[0] == '\0' ||
        plan->item_count > UMI_WORKBENCH_LAYOUT_DATA_MAX_SYNC_ITEMS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < plan->item_count; ++index) {
        if (plan->items[index].structure_size <
                sizeof(plan->items[index]) ||
            plan->items[index].layout_id[0] == '\0') {
            return UMI_STATUS_INVALID_STATE;
        }
        for (nested = index + 1U;
             nested < plan->item_count;
             ++nested) {
            if (strcmp(plan->items[index].layout_id,
                       plan->items[nested].layout_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}

const UmiWorkbenchLayoutSyncPlanItem *
umi_workbench_layout_sync_plan_find(
    const UmiWorkbenchLayoutSyncPlan *plan,
    const char *layout_id)
{
    size_t index;
    if (plan == NULL || layout_id == NULL) return NULL;
    for (index = 0U; index < plan->item_count; ++index) {
        if (strcmp(plan->items[index].layout_id, layout_id) == 0) {
            return &plan->items[index];
        }
    }
    return NULL;
}

size_t umi_workbench_layout_sync_plan_count_action(
    const UmiWorkbenchLayoutSyncPlan *plan,
    UmiWorkbenchLayoutDataSyncAction action)
{
    size_t count = 0U;
    size_t index;
    if (plan == NULL) return 0U;
    for (index = 0U; index < plan->item_count; ++index) {
        if (plan->items[index].action == action) count += 1U;
    }
    return count;
}

static int compare_items(const void *left, const void *right)
{
    const UmiWorkbenchLayoutSyncPlanItem *a =
        (const UmiWorkbenchLayoutSyncPlanItem *)left;
    const UmiWorkbenchLayoutSyncPlanItem *b =
        (const UmiWorkbenchLayoutSyncPlanItem *)right;
    if (a->sequence < b->sequence) return -1;
    if (a->sequence > b->sequence) return 1;
    return strcmp(a->layout_id, b->layout_id);
}

void umi_workbench_layout_sync_plan_sort(
    UmiWorkbenchLayoutSyncPlan *plan)
{
    if (plan == NULL || plan->item_count < 2U) return;
    qsort(plan->items, plan->item_count,
          sizeof(plan->items[0]), compare_items);
}
