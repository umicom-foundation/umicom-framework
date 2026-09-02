/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/reliability.h
 *
 * PURPOSE:
 *   Define deterministic retry policies, status classification and exponential delay calculations for reliable message delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_RELIABILITY_H
#define UMICOM_MESSAGING_RELIABILITY_H

#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the retry policy data shared with callers of this public contract.
 */
typedef struct UmiRetryPolicy {
    uint32_t maximum_attempts;
    uint32_t initial_delay_ms;
    uint32_t maximum_delay_ms;
    uint32_t multiplier_percent;
} UmiRetryPolicy;

/**
 * Provide the retry policy default operation used by this module and its client
 * applications.
 */
UmiRetryPolicy umi_retry_policy_default(void);
/**
 * Provide the retry status is retryable operation used by this module and its client
 * applications.
 */
int umi_retry_status_is_retryable(UmiStatus status);
/**
 * Provide the retry should attempt operation used by this module and its client
 * applications.
 */
int umi_retry_should_attempt(const UmiRetryPolicy *policy,
                             uint32_t completed_attempts,
                             UmiStatus last_status);
/**
 * Provide the retry delay ms operation used by this module and its client applications.
 */
uint32_t umi_retry_delay_ms(const UmiRetryPolicy *policy,
                            uint32_t completed_attempts);

#ifdef __cplusplus
}
#endif

#endif
