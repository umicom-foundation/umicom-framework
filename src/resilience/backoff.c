/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/backoff.c
 *
 * PURPOSE:
 *   Implement overflow-safe retry delay calculation and bounded deterministic
 *   jitter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/backoff.h"

#include <stddef.h>
#include <stdint.h>
UmiBackoffPolicy umi_backoff_policy_default(void)
{ UmiBackoffPolicy p = {UMI_BACKOFF_EXPONENTIAL, 100U, 30000U, 2U, 10U}; return p; }
uint32_t umi_backoff_delay(const UmiBackoffPolicy *policy, uint32_t attempt, uint32_t jitter_seed)
{
    uint64_t delay; uint32_t i, jitter, span;
    if (policy == NULL || policy->initial_delay_ms == 0U) return 0U;
    delay = policy->initial_delay_ms;
    if (policy->kind == UMI_BACKOFF_LINEAR) delay += (uint64_t)policy->initial_delay_ms * attempt;
    else if (policy->kind == UMI_BACKOFF_EXPONENTIAL) for (i = 0U; i < attempt; ++i) { delay *= policy->multiplier > 1U ? policy->multiplier : 2U; if (delay >= policy->maximum_delay_ms) break; }
    if (policy->maximum_delay_ms > 0U && delay > policy->maximum_delay_ms) delay = policy->maximum_delay_ms;
    span = (uint32_t)((delay * policy->jitter_percent) / 100U);
    if (span > 0U) { jitter = jitter_seed % (span * 2U + 1U); delay = delay > span ? delay - span + jitter : jitter; }
    if (policy->maximum_delay_ms > 0U && delay > policy->maximum_delay_ms) delay = policy->maximum_delay_ms;
    return (uint32_t)delay;
}
