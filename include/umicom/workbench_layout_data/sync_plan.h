/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/sync_plan.h
 *
 * PURPOSE:
 *   Build deterministic bidirectional synchronisation plans from local and remote change feeds without mutating either replica.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_SYNC_PLAN_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_SYNC_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutSyncPlanningInput {
    uint32_t structure_size;
    const UmiWorkbenchLayoutChange *local_changes;
    size_t local_change_count;
    const UmiWorkbenchLayoutChange *remote_changes;
    size_t remote_change_count;
    const UmiWorkbenchLayoutSyncCursor *cursor;
    UmiWorkbenchLayoutDataSyncDirection direction;
    const char *local_replica_id;
    const char *remote_replica_id;
    uint64_t created_at_ms;
} UmiWorkbenchLayoutSyncPlanningInput;

UmiStatus umi_workbench_layout_sync_plan_build(
    const UmiWorkbenchLayoutSyncPlanningInput *input,
    UmiWorkbenchLayoutSyncPlan *out_plan);

UmiStatus umi_workbench_layout_sync_plan_validate(
    const UmiWorkbenchLayoutSyncPlan *plan);

UmiStatus umi_workbench_layout_sync_plan_add_item(
    UmiWorkbenchLayoutSyncPlan *plan,
    const UmiWorkbenchLayoutSyncPlanItem *item);

const UmiWorkbenchLayoutSyncPlanItem *
umi_workbench_layout_sync_plan_find(
    const UmiWorkbenchLayoutSyncPlan *plan,
    const char *layout_id);

size_t umi_workbench_layout_sync_plan_count_action(
    const UmiWorkbenchLayoutSyncPlan *plan,
    UmiWorkbenchLayoutDataSyncAction action);

void umi_workbench_layout_sync_plan_sort(
    UmiWorkbenchLayoutSyncPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
