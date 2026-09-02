/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/rollback.h
 *
 * PURPOSE:
 *   Represent and validate rollback requests between immutable installed generations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Rollback is an explicit operation with a reason and approval state rather than an ad-hoc file copy.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_ROLLBACK_H
#define INCLUDE_UMICOM_DELIVERY_ROLLBACK_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the rollback plan data shared with callers of this public contract.
 */
typedef struct UmiRollbackPlan {
    uint64_t current_generation;
    uint64_t target_generation;
    char reason[UMI_DELIVERY_TEXT_CAPACITY];
    int approved;
} UmiRollbackPlan;

/**
 * Initialise rollback plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rollback_plan_init(UmiRollbackPlan *plan,
                                 uint64_t current_generation,
                                 uint64_t target_generation,
                                 const char *reason);
/**
 * Provide the rollback plan approve operation used by this module and its client
 * applications.
 */
UmiStatus umi_rollback_plan_approve(UmiRollbackPlan *plan);
/**
 * Check that rollback plan satisfies its contract before another service relies on it.
 */
int umi_rollback_plan_valid(const UmiRollbackPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
