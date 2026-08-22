/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/delivery.c
 *
 * PURPOSE:
 *   Implement context-delivery initialisation and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/delivery.h"
#include <string.h>
void umi_workbench_context_host_delivery_init(
    UmiWorkbenchContextHostDelivery *delivery,
    const char *delivery_id)
{
    if (delivery == NULL) return;
    memset(delivery,0,sizeof(*delivery));
    delivery->structure_size=(uint32_t)sizeof(*delivery);
    delivery->disposition=UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_QUEUED;
    delivery->revision=1U;
    if(delivery_id!=NULL)(void)umi_workbench_context_host_copy_text(
        delivery->delivery_id,sizeof(delivery->delivery_id),delivery_id);
}
UmiStatus umi_workbench_context_host_delivery_validate(
    const UmiWorkbenchContextHostDelivery *delivery)
{
    if(delivery==NULL||delivery->structure_size!=sizeof(*delivery))
        return UMI_STATUS_INVALID_ARGUMENT;
    if(delivery->delivery_id[0]=='\0'||delivery->endpoint_id[0]=='\0'||
       delivery->panel_id[0]=='\0'||delivery->application_id[0]=='\0'||
       delivery->group_id[0]=='\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_context_payload_validate(&delivery->payload)!=UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
