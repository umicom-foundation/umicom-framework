/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/compensation_plan.h
 * PURPOSE: Model approved compensating actions for Helix v2 rollback.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Compensation actions are returned in reverse order, matching
 * the normal stack discipline used to undo a sequence safely. */
#ifndef INCLUDE_UMICOM_HELIX_COMPENSATION_PLAN_H
#define INCLUDE_UMICOM_HELIX_COMPENSATION_PLAN_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/helix/action.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named helix compensation state values accepted by this public contract.
 */
typedef enum UmiHelixCompensationState {
    UMI_HELIX_ROLLBACK_EMPTY = 1,
    UMI_HELIX_ROLLBACK_READY = 2,
    UMI_HELIX_ROLLBACK_RUNNING = 3,
    UMI_HELIX_ROLLBACK_COMPLETE = 4,
    UMI_HELIX_ROLLBACK_FAILED = 5
} UmiHelixCompensationState;

/**
 * Represent the helix compensation plan data shared with callers of this public contract.
 */
typedef struct UmiHelixCompensationPlan {
    UmiHelixAction compensations[UMI_HELIX_RUNTIME_MAX_COMPENSATIONS];
    size_t count;
    size_t remaining;
    UmiHelixCompensationState state;
} UmiHelixCompensationPlan;

/**
 * Initialise helix compensation plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_helix_compensation_plan_init(UmiHelixCompensationPlan *plan);
/**
 * Add helix compensation plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_helix_compensation_plan_add(UmiHelixCompensationPlan *plan,
                                   const UmiHelixAction *compensation);
/**
 * Provide the helix compensation plan begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_compensation_plan_begin(UmiHelixCompensationPlan *plan,
                                     int approved);
/**
 * Provide the helix compensation plan next operation used by this module and its client
 * applications.
 */
const UmiHelixAction *umi_helix_compensation_plan_next(
    UmiHelixCompensationPlan *plan);
/**
 * Provide the helix compensation plan record operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_compensation_plan_record(UmiHelixCompensationPlan *plan,
                                      UmiStatus result);

#ifdef __cplusplus
}
#endif

#endif
