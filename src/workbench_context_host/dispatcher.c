/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/dispatcher.c
 *
 * PURPOSE:
 *   Implement deterministic delivery batching into panel inboxes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/dispatcher.h"
#include <stdio.h>
#include <string.h>
/*
 * Perform workbench context host through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_workbench_context_host_dispatch(
    UmiWorkbenchContextHostEndpointRegistry *endpoints,
    UmiWorkbenchContextHostInboxRegistry *inboxes,
    UmiWorkbenchContextHostMetrics *metrics,
    const UmiWorkbenchContextLinkDeliveryBatch *batch,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms)
{
    size_t i;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!endpoints||!inboxes||!metrics||!batch||!payload)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<batch->count;++i){
        const UmiWorkbenchContextLinkDelivery *route=&batch->items[i];
        const UmiWorkbenchContextHostEndpoint *endpoint=
            umi_workbench_context_host_endpoint_registry_find_panel(endpoints,route->panel_id);
        UmiWorkbenchContextHostInbox *inbox;
        UmiWorkbenchContextHostDelivery delivery;
        UmiStatus s;
        char delivery_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
        int written;
        /* Apply this branch only when its contract condition is satisfied. */
        if(!endpoint||!umi_workbench_context_host_endpoint_accepts(endpoint,payload->kind)){
            ++metrics->delivery_dropped_count;continue;
        }
        s=umi_workbench_context_host_inbox_registry_ensure(
            inboxes,endpoint->endpoint_id,endpoint->panel_id,&inbox);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if(s!=UMI_STATUS_OK){++metrics->error_count;return s;}
        written=snprintf(delivery_id,sizeof(delivery_id),"%s-%llu",
                         endpoint->endpoint_id,(unsigned long long)route->sequence);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if(written<0||(size_t)written>=sizeof(delivery_id)){
            ++metrics->error_count;return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        umi_workbench_context_host_delivery_init(&delivery,delivery_id);
        (void)umi_workbench_context_host_copy_text(
            delivery.endpoint_id,sizeof(delivery.endpoint_id),endpoint->endpoint_id);
        (void)umi_workbench_context_host_copy_text(
            delivery.panel_id,sizeof(delivery.panel_id),endpoint->panel_id);
        (void)umi_workbench_context_host_copy_text(
            delivery.application_id,sizeof(delivery.application_id),endpoint->application_id);
        (void)umi_workbench_context_host_copy_text(
            delivery.group_id,sizeof(delivery.group_id),route->group_id);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(source_panel_id)(void)umi_workbench_context_host_copy_text(
            delivery.source_panel_id,sizeof(delivery.source_panel_id),source_panel_id);
        delivery.payload=*payload;delivery.sequence=route->sequence;delivery.queued_at_ms=now_ms;
        s=umi_workbench_context_host_inbox_push(inbox,&delivery);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if(s!=UMI_STATUS_OK){++metrics->error_count;return s;}
        ++metrics->delivery_queued_count;
    }
    ++metrics->revision;
    return UMI_STATUS_OK;
}
