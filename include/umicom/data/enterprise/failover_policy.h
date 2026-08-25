/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/failover_policy.h
 *
 * PURPOSE:
 *   Gate replica promotion based on health, lag and explicit promotion permission.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_FAILOVER_POLICY_H
#define UMICOM_DATA_ENTERPRISE_FAILOVER_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataFailoverPolicy {
    uint64_t maximum_promotion_lag_ms;
    bool automatic_failover;
    bool require_manual_approval;
} UmiDataFailoverPolicy;

/* Initialise conservative Framework defaults for this policy. */
void umi_data_failover_policy_default(UmiDataFailoverPolicy *policy);
/* Evaluate the policy against operation evidence without performing I/O. */
UmiStatus umi_data_failover_policy_evaluate(const UmiDataFailoverPolicy *policy, bool candidate_healthy, uint64_t lag_ms, bool approved, bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
