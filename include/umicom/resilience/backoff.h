/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/backoff.h
 *
 * PURPOSE:
 *   Calculate bounded linear and exponential retry delays with deterministic
 *   jitter inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_BACKOFF_H
#define UMICOM_RESILIENCE_BACKOFF_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named backoff kind values accepted by this public contract.
 */
typedef enum UmiBackoffKind { UMI_BACKOFF_FIXED = 1, UMI_BACKOFF_LINEAR = 2, UMI_BACKOFF_EXPONENTIAL = 3 } UmiBackoffKind;
/**
 * Represent the backoff policy data shared with callers of this public contract.
 */
typedef struct UmiBackoffPolicy { UmiBackoffKind kind; uint32_t initial_delay_ms; uint32_t maximum_delay_ms; uint32_t multiplier; uint32_t jitter_percent; } UmiBackoffPolicy;
/**
 * Provide the backoff policy default operation used by this module and its client
 * applications.
 */
UmiBackoffPolicy umi_backoff_policy_default(void);
/**
 * Provide the backoff delay operation used by this module and its client applications.
 */
uint32_t umi_backoff_delay(const UmiBackoffPolicy *policy,
                           uint32_t attempt,
                           uint32_t jitter_seed);
#ifdef __cplusplus
}
#endif

#endif
