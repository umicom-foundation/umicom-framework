/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/delivery.c
 *
 * PURPOSE:
 *   Implement context-delivery initialisation and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/delivery.h"
#include <string.h>
/*
 * Initialise workbench context host delivery from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_delivery_init(
    UmiWorkbenchContextHostDelivery *delivery,
    const char *delivery_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (delivery == NULL) return;
    memset(delivery,0,sizeof(*delivery));
    delivery->structure_size=(uint32_t)sizeof(*delivery);
    delivery->disposition=UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_QUEUED;
    delivery->revision=1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(delivery_id!=NULL)(void)umi_workbench_context_host_copy_text(
        delivery->delivery_id,sizeof(delivery->delivery_id),delivery_id);
}
/*
 * Check that workbench context host delivery satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_host_delivery_validate(
    const UmiWorkbenchContextHostDelivery *delivery)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(delivery==NULL||delivery->structure_size!=sizeof(*delivery))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(delivery->delivery_id[0]=='\0'||delivery->endpoint_id[0]=='\0'||
       delivery->panel_id[0]=='\0'||delivery->application_id[0]=='\0'||
       delivery->group_id[0]=='\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_payload_validate(&delivery->payload)!=UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
