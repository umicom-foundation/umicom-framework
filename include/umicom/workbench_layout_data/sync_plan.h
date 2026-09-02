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


/**
 * Represent the workbench layout sync planning input data shared with callers of this
 * public contract.
 */
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

/**
 * Provide the workbench layout sync plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_sync_plan_build(
    const UmiWorkbenchLayoutSyncPlanningInput *input,
    UmiWorkbenchLayoutSyncPlan *out_plan);

/**
 * Check that workbench layout sync plan satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_layout_sync_plan_validate(
    const UmiWorkbenchLayoutSyncPlan *plan);

/**
 * Provide the workbench layout sync plan add item operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_sync_plan_add_item(
    UmiWorkbenchLayoutSyncPlan *plan,
    const UmiWorkbenchLayoutSyncPlanItem *item);

/**
 * Find workbench layout sync plan while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchLayoutSyncPlanItem *
umi_workbench_layout_sync_plan_find(
    const UmiWorkbenchLayoutSyncPlan *plan,
    const char *layout_id);

/**
 * Provide the workbench layout sync plan count action operation used by this module and
 * its client applications.
 */
size_t umi_workbench_layout_sync_plan_count_action(
    const UmiWorkbenchLayoutSyncPlan *plan,
    UmiWorkbenchLayoutDataSyncAction action);

/**
 * Provide the workbench layout sync plan sort operation used by this module and its client
 * applications.
 */
void umi_workbench_layout_sync_plan_sort(
    UmiWorkbenchLayoutSyncPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
