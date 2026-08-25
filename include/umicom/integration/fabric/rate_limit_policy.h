/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/rate_limit_policy.h
 *
 * PURPOSE:
 *   Apply a deterministic fixed-window rate-limit policy using caller-supplied counters and time.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiFabricRateLimitPolicy { uint32_t maximum_requests; uint64_t window_ms; } UmiFabricRateLimitPolicy;
void umi_fabric_rate_limit_policy_default(UmiFabricRateLimitPolicy *policy);
UmiStatus umi_fabric_rate_limit_policy_evaluate(const UmiFabricRateLimitPolicy *policy,uint32_t requests_in_window,bool *out_allowed,uint32_t *out_remaining);

#ifdef __cplusplus
}
#endif
#endif
