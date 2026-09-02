/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/rate_limiter.c
 *
 * PURPOSE:
 *   Implement a portable token bucket with monotonic-time refill accounting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/rate_limiter.h"

#include <stddef.h>
#include <stdlib.h>
#include "umicom/platform/threading.h"
struct UmiRateLimiter { UmiRateLimiterConfig config; UmiRateLimiterSnapshot state; uint64_t last_refill_ns; UmiMutex *mutex; };
/*
 * Initialise rate limiter from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rate_limiter_create(const UmiRateLimiterConfig *config, uint64_t now_ns, UmiRateLimiter **out_limiter)
{
    UmiRateLimiter *l; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_limiter == NULL || config->capacity <= 0.0 || config->refill_per_second < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_limiter = NULL; l = (UmiRateLimiter *)calloc(1U, sizeof(*l)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (l == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&l->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(l); return status; }
    l->config = *config; l->state.tokens = config->capacity; l->last_refill_ns = now_ns; *out_limiter = l; return UMI_STATUS_OK;
}
/* Release or reset state held by rate limiter so the same storage can be reused safely. */
void umi_rate_limiter_destroy(UmiRateLimiter *limiter) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (limiter != NULL) { umi_mutex_destroy(limiter->mutex); free(limiter); } }
/*
 * Provide the rate limiter allow operation used by this module and its client
 * applications.
 */
int umi_rate_limiter_allow(UmiRateLimiter *limiter, double tokens, uint64_t now_ns)
{
    double elapsed; int allowed = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (limiter == NULL || tokens <= 0.0) return 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_mutex_lock(limiter->mutex) != UMI_STATUS_OK) return 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (now_ns > limiter->last_refill_ns) { elapsed = (double)(now_ns - limiter->last_refill_ns) / 1000000000.0; limiter->state.tokens += elapsed * limiter->config.refill_per_second; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (limiter->state.tokens > limiter->config.capacity) limiter->state.tokens = limiter->config.capacity; limiter->last_refill_ns = now_ns; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limiter->state.tokens >= tokens) { limiter->state.tokens -= tokens; ++limiter->state.accepted; allowed = 1; } /* Use this fallback path when the earlier condition does not apply. */ else ++limiter->state.rejected;
    (void)umi_mutex_unlock(limiter->mutex); return allowed;
}
/*
 * Provide the rate limiter snapshot operation used by this module and its client
 * applications.
 */
UmiRateLimiterSnapshot umi_rate_limiter_snapshot(UmiRateLimiter *limiter)
{ UmiRateLimiterSnapshot s = {0}; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (limiter != NULL && umi_mutex_lock(limiter->mutex) == UMI_STATUS_OK) { s = limiter->state; (void)umi_mutex_unlock(limiter->mutex); } return s; }
