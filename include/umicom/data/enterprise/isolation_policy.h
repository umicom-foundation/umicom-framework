/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/isolation_policy.h
 *
 * PURPOSE:
 *   Gate operations by requested transaction isolation strength.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_ISOLATION_POLICY_H
#define UMICOM_DATA_ENTERPRISE_ISOLATION_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataIsolationPolicy {
    uint32_t minimum_level;
    uint32_t maximum_level;
    bool allow_upgrade;
} UmiDataIsolationPolicy;

/* Initialise conservative Framework defaults for this policy. */
void umi_data_isolation_policy_default(UmiDataIsolationPolicy *policy);
/* Evaluate the policy against operation evidence without performing I/O. */
UmiStatus umi_data_isolation_policy_evaluate(const UmiDataIsolationPolicy *policy, uint32_t requested_level, bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
