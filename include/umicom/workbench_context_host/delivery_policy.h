/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/delivery_policy.h
 *
 * PURPOSE:
 *   Define bounded delivery pressure and replacement policy independent of frontend toolkits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_DELIVERY_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_DELIVERY_POLICY_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named workbench context host overflow mode values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostOverflowMode {
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_OLDEST=1,
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_NEWEST=2,
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_REJECT=3
} UmiWorkbenchContextHostOverflowMode;
/**
 * Represent the workbench context host delivery policy data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostDeliveryPolicy {
    size_t max_pending_per_endpoint;
    UmiWorkbenchContextHostOverflowMode overflow_mode;
    bool coalesce_same_kind;
    bool coalesce_same_context;
    bool reject_expired;
    uint64_t revision;
} UmiWorkbenchContextHostDeliveryPolicy;
/**
 * Provide the workbench context host delivery policy default operation used by this module
 * and its client applications.
 */
UmiWorkbenchContextHostDeliveryPolicy umi_workbench_context_host_delivery_policy_default(void);
/**
 * Check that workbench context host delivery policy satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_host_delivery_policy_validate(
    const UmiWorkbenchContextHostDeliveryPolicy *policy);
#ifdef __cplusplus
}
#endif
#endif
