/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/reliability.c
 *
 * PURPOSE:
 *   Implement retryability classification, bounded attempt decisions and overflow-safe exponential delay calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/reliability.h"

#include <stddef.h>
#include <stdint.h>

/*
 * Provide the retry policy default operation used by this module and its client
 * applications.
 */
UmiRetryPolicy umi_retry_policy_default(void)
{
    UmiRetryPolicy policy;
    policy.maximum_attempts = 5U;
    policy.initial_delay_ms = 100U;
    policy.maximum_delay_ms = 5000U;
    policy.multiplier_percent = 200U;
    return policy;
}

/*
 * Provide the retry status is retryable operation used by this module and its client
 * applications.
 */
int umi_retry_status_is_retryable(UmiStatus status)
{
    return status == UMI_STATUS_UNAVAILABLE ||
           status == UMI_STATUS_TIMEOUT ||
           status == UMI_STATUS_BUSY ||
           status == UMI_STATUS_IO_ERROR;
}

/*
 * Provide the retry should attempt operation used by this module and its client
 * applications.
 */
int umi_retry_should_attempt(const UmiRetryPolicy *policy,
                             uint32_t completed_attempts,
                             UmiStatus last_status)
{
    UmiRetryPolicy effective = policy != NULL
        ? *policy
        : umi_retry_policy_default();
    return completed_attempts < effective.maximum_attempts &&
           umi_retry_status_is_retryable(last_status);
}

/* Provide the retry delay ms operation used by this module and its client applications. */
uint32_t umi_retry_delay_ms(const UmiRetryPolicy *policy,
                            uint32_t completed_attempts)
{
    UmiRetryPolicy effective = policy != NULL
        ? *policy
        : umi_retry_policy_default();
    uint64_t delay = effective.initial_delay_ms;
    uint32_t attempt;
    /* Visit each bounded item once so every record receives the same rule. */
    for (attempt = 0U; attempt < completed_attempts; ++attempt) {
        delay = (delay * effective.multiplier_percent) / 100U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (delay >= effective.maximum_delay_ms) {
            return effective.maximum_delay_ms;
        }
    }
    return delay > UINT32_MAX ? UINT32_MAX : (uint32_t)delay;
}
