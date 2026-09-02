/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/retry.h
 *
 * PURPOSE:
 *   Execute bounded retryable operations with explicit status classification,
 *   backoff and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_RETRY_H
#define UMICOM_RESILIENCE_RETRY_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include "umicom/resilience/backoff.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiResilienceRetryOperation)(void *user_data, uint32_t attempt);
typedef int (*UmiResilienceRetryableStatusFn)(UmiStatus status, void *user_data);
/**
 * Represent the resilience retry policy data shared with callers of this public contract.
 */
typedef struct UmiResilienceRetryPolicy { uint32_t maximum_attempts; UmiBackoffPolicy backoff; int sleep_between_attempts; UmiResilienceRetryableStatusFn retryable; void *retryable_user_data; } UmiResilienceRetryPolicy;
/**
 * Represent the resilience retry result data shared with callers of this public contract.
 */
typedef struct UmiResilienceRetryResult { UmiStatus status; uint32_t attempts; uint32_t total_delay_ms; } UmiResilienceRetryResult;
/**
 * Provide the resilience retry policy default operation used by this module and its client
 * applications.
 */
UmiResilienceRetryPolicy umi_resilience_retry_policy_default(void);
/**
 * Perform resilience retry through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_resilience_retry_execute(const UmiResilienceRetryPolicy *policy,
                            UmiResilienceRetryOperation operation,
                            void *operation_user_data,
                            UmiCancellationToken *cancellation,
                            UmiResilienceRetryResult *out_result);
#ifdef __cplusplus
}
#endif

#endif
