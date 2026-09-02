/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/rate_limit_policy.h
 *
 * PURPOSE:
 *   Apply a deterministic fixed-window rate-limit policy using caller-supplied counters and time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_RATE_LIMIT_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_RATE_LIMIT_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric rate limit policy data shared with callers of this public contract.
 */
typedef struct UmiFabricRateLimitPolicy { uint32_t maximum_requests; uint64_t window_ms; } UmiFabricRateLimitPolicy;
/**
 * Provide the fabric rate limit policy default operation used by this module and its
 * client applications.
 */
void umi_fabric_rate_limit_policy_default(UmiFabricRateLimitPolicy *policy);
/**
 * Provide the fabric rate limit policy evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_rate_limit_policy_evaluate(const UmiFabricRateLimitPolicy *policy,uint32_t requests_in_window,bool *out_allowed,uint32_t *out_remaining);

#ifdef __cplusplus
}
#endif
#endif
