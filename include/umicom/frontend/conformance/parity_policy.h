/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/parity_policy.h
 *
 * PURPOSE:
 *   minimum parity thresholds and blocker policy for release decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_PARITY_POLICY_H
#define UMICOM_FRONTEND_CONFORMANCE_PARITY_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc parity policy data shared with callers of this public contract.
 */
typedef struct UmiFcParityPolicy { double minimum_score; double minimum_accessibility; double minimum_layout; size_t maximum_degraded; bool blockers_forbidden; } UmiFcParityPolicy;
/**
 * Provide the fc parity policy default operation used by this module and its client
 * applications.
 */
void umi_fc_parity_policy_default(UmiFcParityPolicy *policy);
/**
 * Provide the fc parity policy accepts operation used by this module and its client
 * applications.
 */
bool umi_fc_parity_policy_accepts(const UmiFcParityPolicy *policy,double score,double accessibility,double layout,size_t degraded,size_t blockers);

#ifdef __cplusplus
}
#endif
#endif
