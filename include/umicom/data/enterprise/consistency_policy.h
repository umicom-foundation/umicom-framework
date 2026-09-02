/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/consistency_policy.h
 *
 * PURPOSE:
 *   Decide whether observed replica lag is acceptable for a requested consistency model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_CONSISTENCY_POLICY_H
#define UMICOM_DATA_ENTERPRISE_CONSISTENCY_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data consistency policy data shared with callers of this public contract.
 */
typedef struct UmiDataConsistencyPolicy {
    UmiDataConsistency minimum_consistency;
    uint64_t session_lag_ms;
    uint64_t eventual_lag_ms;
} UmiDataConsistencyPolicy;

/* Initialise conservative Framework defaults for this policy. */
void umi_data_consistency_policy_default(UmiDataConsistencyPolicy *policy);
/* Evaluate the policy against operation evidence without performing I/O. */
UmiStatus umi_data_consistency_policy_evaluate(const UmiDataConsistencyPolicy *policy, UmiDataConsistency requested, uint64_t observed_lag_ms, bool is_primary, bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
