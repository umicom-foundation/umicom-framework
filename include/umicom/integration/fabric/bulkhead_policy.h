/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/bulkhead_policy.h
 *
 * PURPOSE:
 *   Enforce bounded concurrency and queue capacity before integration work enters shared executors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_BULKHEAD_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_BULKHEAD_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric bulkhead policy data shared with callers of this public contract.
 */
typedef struct UmiFabricBulkheadPolicy { uint32_t maximum_concurrent; uint32_t maximum_queued; } UmiFabricBulkheadPolicy;
/**
 * Provide the fabric bulkhead policy default operation used by this module and its client
 * applications.
 */
void umi_fabric_bulkhead_policy_default(UmiFabricBulkheadPolicy *policy);
/**
 * Provide the fabric bulkhead policy evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_bulkhead_policy_evaluate(const UmiFabricBulkheadPolicy *policy,uint32_t active,uint32_t queued,bool *out_admit,bool *out_queue);

#ifdef __cplusplus
}
#endif
#endif
