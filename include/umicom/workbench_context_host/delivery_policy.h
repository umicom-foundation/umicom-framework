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
typedef enum UmiWorkbenchContextHostOverflowMode {
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_OLDEST=1,
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_NEWEST=2,
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_REJECT=3
} UmiWorkbenchContextHostOverflowMode;
typedef struct UmiWorkbenchContextHostDeliveryPolicy {
    size_t max_pending_per_endpoint;
    UmiWorkbenchContextHostOverflowMode overflow_mode;
    bool coalesce_same_kind;
    bool coalesce_same_context;
    bool reject_expired;
    uint64_t revision;
} UmiWorkbenchContextHostDeliveryPolicy;
UmiWorkbenchContextHostDeliveryPolicy umi_workbench_context_host_delivery_policy_default(void);
UmiStatus umi_workbench_context_host_delivery_policy_validate(
    const UmiWorkbenchContextHostDeliveryPolicy *policy);
#ifdef __cplusplus
}
#endif
#endif
