/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/backoff.c
 *
 * PURPOSE:
 *   Implement overflow-safe retry delay calculation and bounded deterministic
 *   jitter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/backoff.h"

#include <stddef.h>
#include <stdint.h>
/*
 * Provide the backoff policy default operation used by this module and its client
 * applications.
 */
UmiBackoffPolicy umi_backoff_policy_default(void)
{ UmiBackoffPolicy p = {UMI_BACKOFF_EXPONENTIAL, 100U, 30000U, 2U, 10U}; return p; }
/* Provide the backoff delay operation used by this module and its client applications. */
uint32_t umi_backoff_delay(const UmiBackoffPolicy *policy, uint32_t attempt, uint32_t jitter_seed)
{
    uint64_t delay; uint32_t i, jitter, span;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || policy->initial_delay_ms == 0U) return 0U;
    delay = policy->initial_delay_ms;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->kind == UMI_BACKOFF_LINEAR) delay += (uint64_t)policy->initial_delay_ms * attempt;
    else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (policy->kind == UMI_BACKOFF_EXPONENTIAL) /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < attempt; ++i) { delay *= policy->multiplier > 1U ? policy->multiplier : 2U; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (delay >= policy->maximum_delay_ms) break; }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->maximum_delay_ms > 0U && delay > policy->maximum_delay_ms) delay = policy->maximum_delay_ms;
    span = (uint32_t)((delay * policy->jitter_percent) / 100U);
    /* Apply this branch only when its contract condition is satisfied. */
    if (span > 0U) { jitter = jitter_seed % (span * 2U + 1U); delay = delay > span ? delay - span + jitter : jitter; }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->maximum_delay_ms > 0U && delay > policy->maximum_delay_ms) delay = policy->maximum_delay_ms;
    return (uint32_t)delay;
}
