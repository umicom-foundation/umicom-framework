/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/cleanup.c
 *
 * PURPOSE:
 *   Collect cleanup candidates and total reclaimable bytes without deleting them automatically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Cleanup is planned first so Studio can show exactly what would be removed before destructive actions are approved.
 */

#include "umicom/delivery/cleanup.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise cleanup plan from caller-provided values so later operations receive a known
 * state.
 */
void umi_cleanup_plan_init(UmiCleanupPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) (void)memset(plan, 0, sizeof(*plan));
}
/* Add cleanup plan only after its inputs and available capacity have been checked. */
UmiStatus umi_cleanup_plan_add(UmiCleanupPlan *plan,
                               const char *path,
                               uint64_t bytes)
{
    UmiCleanupCandidate *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->count >= UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &plan->items[plan->count];
    (void)memset(item, 0, sizeof(*item));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(item->path, sizeof(item->path), path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item->bytes = bytes;
    plan->reclaimable_bytes += bytes;
    ++plan->count;
    return UMI_STATUS_OK;
}
