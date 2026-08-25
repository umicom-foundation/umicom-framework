/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/pool_policy.h
 *
 * PURPOSE:
 *   Gate pool expansion, leasing and retirement using explicit capacity thresholds.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_POOL_POLICY_H
#define UMICOM_DATA_ENTERPRISE_POOL_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataPoolPolicy {
    size_t minimum_size;
    size_t maximum_size;
    double high_utilisation_percent;
} UmiDataPoolPolicy;

/* Initialise conservative Framework defaults for this policy. */
void umi_data_pool_policy_default(UmiDataPoolPolicy *policy);
/* Evaluate the policy against operation evidence without performing I/O. */
UmiStatus umi_data_pool_policy_evaluate(const UmiDataPoolPolicy *policy, size_t current_size, double utilisation_percent, bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
