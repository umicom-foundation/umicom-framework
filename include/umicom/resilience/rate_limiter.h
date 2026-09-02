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
/**
 * Represent the rate limiter config data shared with callers of this public contract.
 */
typedef struct UmiRateLimiterConfig { double capacity; double refill_per_second; } UmiRateLimiterConfig;
/**
 * Represent the rate limiter snapshot data shared with callers of this public contract.
 */
typedef struct UmiRateLimiterSnapshot { double tokens; uint64_t accepted; uint64_t rejected; } UmiRateLimiterSnapshot;
/**
 * Represent the rate limiter data shared with callers of this public contract.
 */
typedef struct UmiRateLimiter UmiRateLimiter;
/**
 * Initialise rate limiter from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rate_limiter_create(const UmiRateLimiterConfig *config,
                                  uint64_t now_ns,
                                  UmiRateLimiter **out_limiter);
/**
 * Release or reset state held by rate limiter so the same storage can be reused safely.
 */
void umi_rate_limiter_destroy(UmiRateLimiter *limiter);
/**
 * Provide the rate limiter allow operation used by this module and its client
 * applications.
 */
int umi_rate_limiter_allow(UmiRateLimiter *limiter, double tokens, uint64_t now_ns);
/**
 * Provide the rate limiter snapshot operation used by this module and its client
 * applications.
 */
UmiRateLimiterSnapshot umi_rate_limiter_snapshot(UmiRateLimiter *limiter);
#ifdef __cplusplus
}
#endif

#endif
