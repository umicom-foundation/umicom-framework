/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/rollback_v2.h
 * PURPOSE: Model approved compensating actions for Helix v2 rollback.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Compensation actions are returned in reverse order, matching
 * the normal stack discipline used to undo a sequence safely. */
#ifndef INCLUDE_UMICOM_HELIX_ROLLBACK_V2_H
#define INCLUDE_UMICOM_HELIX_ROLLBACK_V2_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/helix/action_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiHelixRollbackStateV2 {
    UMI_HELIX_ROLLBACK_EMPTY = 1,
    UMI_HELIX_ROLLBACK_READY = 2,
    UMI_HELIX_ROLLBACK_RUNNING = 3,
    UMI_HELIX_ROLLBACK_COMPLETE = 4,
    UMI_HELIX_ROLLBACK_FAILED = 5
} UmiHelixRollbackStateV2;

typedef struct UmiHelixRollbackPlanV2 {
    UmiHelixActionV2 compensations[UMI_HELIX_V2_MAX_COMPENSATIONS];
    size_t count;
    size_t remaining;
    UmiHelixRollbackStateV2 state;
} UmiHelixRollbackPlanV2;

void umi_helix_rollback_v2_init(UmiHelixRollbackPlanV2 *plan);
UmiStatus umi_helix_rollback_v2_add(UmiHelixRollbackPlanV2 *plan,
                                   const UmiHelixActionV2 *compensation);
UmiStatus umi_helix_rollback_v2_begin(UmiHelixRollbackPlanV2 *plan,
                                     int approved);
const UmiHelixActionV2 *umi_helix_rollback_v2_next(
    UmiHelixRollbackPlanV2 *plan);
UmiStatus umi_helix_rollback_v2_record(UmiHelixRollbackPlanV2 *plan,
                                      UmiStatus result);

#ifdef __cplusplus
}
#endif

#endif
