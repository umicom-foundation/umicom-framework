/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/update_policy.h
 *
 * PURPOSE:
 *   automatic/manual update eligibility, metered-network and maintenance-window policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_UPDATE_POLICY_H
#define UMICOM_DISTRIBUTION_RUNTIME_UPDATE_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr update policy data shared with callers of this public contract.
 */
typedef struct UmiDrUpdatePolicy { bool automatic; bool allow_metered; bool maintenance_window; bool require_signature; } UmiDrUpdatePolicy;
/**
 * Initialise dr update policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_update_policy_init(UmiDrUpdatePolicy *policy); bool umi_dr_update_policy_eligible(const UmiDrUpdatePolicy *policy,bool metered,bool in_window,bool signed_update);

#ifdef __cplusplus
}
#endif
#endif
