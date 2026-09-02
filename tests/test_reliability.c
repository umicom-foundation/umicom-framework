/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_reliability.c
 *
 * PURPOSE:
 *   Verify retry status classification, maximum attempts and bounded exponential delay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRetryPolicy policy = umi_retry_policy_default();
    assert(umi_retry_status_is_retryable(UMI_STATUS_TIMEOUT));
    assert(!umi_retry_status_is_retryable(UMI_STATUS_INVALID_ARGUMENT));
    assert(umi_retry_should_attempt(&policy, 0U, UMI_STATUS_BUSY));
    assert(umi_retry_delay_ms(&policy, 0U) == 100U);
    assert(umi_retry_delay_ms(&policy, 2U) == 400U);
    assert(!umi_retry_should_attempt(&policy, policy.maximum_attempts,
                                     UMI_STATUS_BUSY));
    return 0;
}
