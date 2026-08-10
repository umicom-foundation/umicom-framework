/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/reliability.h
 *
 * PURPOSE:
 *   Define deterministic retry policies, status classification and exponential delay calculations for reliable message delivery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_RELIABILITY_H
#define UMICOM_MESSAGING_RELIABILITY_H

#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRetryPolicy {
    uint32_t maximum_attempts;
    uint32_t initial_delay_ms;
    uint32_t maximum_delay_ms;
    uint32_t multiplier_percent;
} UmiRetryPolicy;

UmiRetryPolicy umi_retry_policy_default(void);
int umi_retry_status_is_retryable(UmiStatus status);
int umi_retry_should_attempt(const UmiRetryPolicy *policy,
                             uint32_t completed_attempts,
                             UmiStatus last_status);
uint32_t umi_retry_delay_ms(const UmiRetryPolicy *policy,
                            uint32_t completed_attempts);

#ifdef __cplusplus
}
#endif

#endif
