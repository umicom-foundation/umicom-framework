/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/adaptation_plan.h
 *
 * PURPOSE:
 *   reviewable frontend adaptation operations for unsupported or constrained capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_ADAPTATION_PLAN_H
#define UMICOM_FRONTEND_CONFORMANCE_ADAPTATION_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc adaptation plan data shared with callers of this public contract.
 */
typedef struct UmiFcAdaptationPlan { uint64_t missing; uint64_t emulated; uint64_t hidden; size_t operations; bool review_required; } UmiFcAdaptationPlan;
/**
 * Provide the fc adaptation plan build operation used by this module and its client
 * applications.
 */
void umi_fc_adaptation_plan_build(uint64_t missing,uint64_t emulatable,uint64_t hideable,UmiFcAdaptationPlan *out_plan);
/**
 * Provide the fc adaptation plan outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_adaptation_plan_outcome(const UmiFcAdaptationPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
