/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/compatibility_gate.h
 *
 * PURPOSE:
 *   application launch compatibility gate based on required semantic capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_COMPATIBILITY_GATE_H
#define UMICOM_FRONTEND_CONFORMANCE_COMPATIBILITY_GATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc compatibility gate data shared with callers of this public contract.
 */
typedef struct UmiFcCompatibilityGate { uint64_t required; double minimum_score; bool allow_degraded; } UmiFcCompatibilityGate;
/**
 * Provide the fc compatibility gate evaluate operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_compatibility_gate_evaluate(const UmiFcCompatibilityGate *gate,uint64_t actual,double score);

#ifdef __cplusplus
}
#endif
#endif
