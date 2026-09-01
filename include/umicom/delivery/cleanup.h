/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/cleanup.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_CLEANUP_H
#define INCLUDE_UMICOM_DELIVERY_CLEANUP_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCleanupCandidate {
    char path[UMI_DELIVERY_PATH_CAPACITY];
    uint64_t bytes;
} UmiCleanupCandidate;

typedef struct UmiCleanupPlan {
    UmiCleanupCandidate items[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
    uint64_t reclaimable_bytes;
} UmiCleanupPlan;

void umi_cleanup_plan_init(UmiCleanupPlan *plan);
UmiStatus umi_cleanup_plan_add(UmiCleanupPlan *plan,
                               const char *path,
                               uint64_t bytes);

#ifdef __cplusplus
}
#endif

#endif
