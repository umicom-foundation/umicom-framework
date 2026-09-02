/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/rollback_plan.h
 *
 * PURPOSE:
 *   rollback checkpoint and prior-version restoration policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_ROLLBACK_PLAN_H
#define UMICOM_DISTRIBUTION_RUNTIME_ROLLBACK_PLAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr rollback plan data shared with callers of this public contract.
 */
typedef struct UmiDrRollbackPlan { char id[UMI_DR_ID_CAPACITY]; UmiDrVersion restore_version; char checkpoint_id[UMI_DR_ID_CAPACITY]; bool preserve_user_data; bool verified; } UmiDrRollbackPlan;
/**
 * Initialise dr rollback plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_rollback_plan_init(UmiDrRollbackPlan *value);
/**
 * Check that dr rollback plan satisfies its contract before another service relies on it.
 */
bool umi_dr_rollback_plan_valid(const UmiDrRollbackPlan *value);
/**
 * Provide the dr rollback plan fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_rollback_plan_fingerprint(const UmiDrRollbackPlan *value);

#ifdef __cplusplus
}
#endif
#endif
