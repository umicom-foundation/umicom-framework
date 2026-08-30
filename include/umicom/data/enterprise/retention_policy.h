/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/retention_policy.h
 *
 * PURPOSE:
 *   Decide whether backup evidence is eligible for retention or deletion by age and minimum retained copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_RETENTION_POLICY_H
#define UMICOM_DATA_ENTERPRISE_RETENTION_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataRetentionPolicy {
    uint64_t max_age_ms;
    size_t minimum_copies;
    bool preserve_last_success;
} UmiDataRetentionPolicy;

/* Initialise conservative Framework defaults for this policy. */
void umi_data_retention_policy_default(UmiDataRetentionPolicy *policy);
/* Evaluate the policy against operation evidence without performing I/O. */
UmiStatus umi_data_retention_policy_evaluate(const UmiDataRetentionPolicy *policy, uint64_t age_ms, size_t successful_copies, bool is_last_success, bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
