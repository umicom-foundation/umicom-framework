/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/retry.c
 *
 * PURPOSE:
 *   Implement retry execution with cancellation, retry classification and
 *   optional sleep.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/retry.h"

#include <stddef.h>

#include "umicom/platform/threading.h"
static int retry_default_status(UmiStatus status, void *user_data)
{ (void)user_data; return status == UMI_STATUS_UNAVAILABLE || status == UMI_STATUS_BUSY || status == UMI_STATUS_TIMEOUT || status == UMI_STATUS_IO_ERROR; }
UmiResilienceRetryPolicy umi_resilience_retry_policy_default(void)
{ UmiResilienceRetryPolicy p; p.maximum_attempts = 3U; p.backoff = umi_backoff_policy_default(); p.sleep_between_attempts = 1; p.retryable = retry_default_status; p.retryable_user_data = NULL; return p; }
UmiStatus umi_resilience_retry_execute(const UmiResilienceRetryPolicy *policy, UmiResilienceRetryOperation operation, void *operation_user_data, UmiCancellationToken *cancellation, UmiResilienceRetryResult *out_result)
{
    UmiResilienceRetryPolicy effective; UmiStatus status = UMI_STATUS_INVALID_STATE; uint32_t attempt, delay;
    if (operation == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    effective = policy != NULL ? *policy : umi_resilience_retry_policy_default();
    if (effective.maximum_attempts == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_result->status = UMI_STATUS_INVALID_STATE; out_result->attempts = 0U; out_result->total_delay_ms = 0U;
    for (attempt = 0U; attempt < effective.maximum_attempts; ++attempt) {
        if (cancellation != NULL && umi_cancellation_token_is_requested(cancellation)) { status = UMI_STATUS_CANCELLED; break; }
        status = operation(operation_user_data, attempt); ++out_result->attempts;
        if (status == UMI_STATUS_OK) break;
        if (effective.retryable == NULL || !effective.retryable(status, effective.retryable_user_data) || attempt + 1U >= effective.maximum_attempts) break;
        delay = umi_backoff_delay(&effective.backoff, attempt, attempt + 1U); out_result->total_delay_ms += delay;
        if (effective.sleep_between_attempts && delay > 0U) umi_thread_sleep_ms(delay);
    }
    out_result->status = status; return status;
}
