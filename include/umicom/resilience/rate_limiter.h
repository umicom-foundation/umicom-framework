/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/rate_limiter.h
 *
 * PURPOSE:
 *   Define a token-bucket rate limiter for API, tool, plug-in and external-
 *   provider boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_RATE_LIMITER_H
#define UMICOM_RESILIENCE_RATE_LIMITER_H

#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRateLimiterConfig { double capacity; double refill_per_second; } UmiRateLimiterConfig;
typedef struct UmiRateLimiterSnapshot { double tokens; uint64_t accepted; uint64_t rejected; } UmiRateLimiterSnapshot;
typedef struct UmiRateLimiter UmiRateLimiter;
UmiStatus umi_rate_limiter_create(const UmiRateLimiterConfig *config,
                                  uint64_t now_ns,
                                  UmiRateLimiter **out_limiter);
void umi_rate_limiter_destroy(UmiRateLimiter *limiter);
int umi_rate_limiter_allow(UmiRateLimiter *limiter, double tokens, uint64_t now_ns);
UmiRateLimiterSnapshot umi_rate_limiter_snapshot(UmiRateLimiter *limiter);
#ifdef __cplusplus
}
#endif

#endif
