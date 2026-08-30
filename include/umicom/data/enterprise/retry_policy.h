/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/retry_policy.h
 *
 * PURPOSE:
 *   Bound retry attempts for transient data operations without retrying unsafe work indefinitely.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_RETRY_POLICY_H
#define UMICOM_DATA_ENTERPRISE_RETRY_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataRetryPolicy {
    uint32_t max_attempts;
    uint64_t base_delay_ms;
    uint64_t max_delay_ms;
} UmiDataRetryPolicy;

/* Initialise conservative Framework defaults for this policy. */
void umi_data_retry_policy_default(UmiDataRetryPolicy *policy);
/* Evaluate the policy against operation evidence without performing I/O. */
UmiStatus umi_data_retry_policy_evaluate(const UmiDataRetryPolicy *policy, uint32_t attempt, bool transient_failure, bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
