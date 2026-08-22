/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/delivery_policy.c
 *
 * PURPOSE:
 *   Provide safe default delivery pressure rules and validate custom policies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/delivery_policy.h"
UmiWorkbenchContextHostDeliveryPolicy umi_workbench_context_host_delivery_policy_default(void)
{
    UmiWorkbenchContextHostDeliveryPolicy p;
    p.max_pending_per_endpoint=32U;p.overflow_mode=UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_OLDEST;
    p.coalesce_same_kind=false;p.coalesce_same_context=true;p.reject_expired=true;p.revision=1U;return p;
}
UmiStatus umi_workbench_context_host_delivery_policy_validate(
    const UmiWorkbenchContextHostDeliveryPolicy *policy)
{
    if(!policy||policy->max_pending_per_endpoint==0U||
       policy->max_pending_per_endpoint>UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(policy->overflow_mode<UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_OLDEST||
       policy->overflow_mode>UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_REJECT)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
