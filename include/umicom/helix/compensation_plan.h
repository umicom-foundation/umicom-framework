/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/compensation_plan.h
 * PURPOSE: Model approved compensating actions for Helix v2 rollback.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Compensation actions are returned in reverse order, matching
 * the normal stack discipline used to undo a sequence safely. */
#ifndef INCLUDE_UMICOM_HELIX_COMPENSATION_PLAN_H
#define INCLUDE_UMICOM_HELIX_COMPENSATION_PLAN_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/helix/action.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiHelixCompensationState {
    UMI_HELIX_ROLLBACK_EMPTY = 1,
    UMI_HELIX_ROLLBACK_READY = 2,
    UMI_HELIX_ROLLBACK_RUNNING = 3,
    UMI_HELIX_ROLLBACK_COMPLETE = 4,
    UMI_HELIX_ROLLBACK_FAILED = 5
} UmiHelixCompensationState;

typedef struct UmiHelixCompensationPlan {
    UmiHelixAction compensations[UMI_HELIX_RUNTIME_MAX_COMPENSATIONS];
    size_t count;
    size_t remaining;
    UmiHelixCompensationState state;
} UmiHelixCompensationPlan;

void umi_helix_compensation_plan_init(UmiHelixCompensationPlan *plan);
UmiStatus umi_helix_compensation_plan_add(UmiHelixCompensationPlan *plan,
                                   const UmiHelixAction *compensation);
UmiStatus umi_helix_compensation_plan_begin(UmiHelixCompensationPlan *plan,
                                     int approved);
const UmiHelixAction *umi_helix_compensation_plan_next(
    UmiHelixCompensationPlan *plan);
UmiStatus umi_helix_compensation_plan_record(UmiHelixCompensationPlan *plan,
                                      UmiStatus result);

#ifdef __cplusplus
}
#endif

#endif
