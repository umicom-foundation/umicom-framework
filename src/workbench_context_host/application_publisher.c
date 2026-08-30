/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/application_publisher.c
 *
 * PURPOSE:
 *   Publish application-launcher selections for Desk and cross-application navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/application_publisher.h"
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_host/payload.h"
UmiStatus umi_workbench_context_host_publish_application(
    UmiWorkbenchContextHost *host,const char *group_id,const char *source_panel_id,
    const char *context_id,const char *application_target_id,const char *runtime_state,
    const char *taskbar_group,const char *layout_id,uint64_t now_ms)
{
    UmiContextPayload p;UmiStatus s;
    if(!host||!context_id||!application_target_id||!runtime_state||!taskbar_group||!layout_id)
        return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_link_build_selection(&p,context_id,host->application_id,
        source_panel_id?source_panel_id:"application-launcher",application_target_id,
        "application",application_target_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"runtime-state",runtime_state);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"taskbar-group",taskbar_group);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"layout-id",layout_id);if(s!=UMI_STATUS_OK)return s;
    p.audit.published_at_ms=now_ms;umi_context_payload_refresh_hash(&p);
    return umi_workbench_context_host_publish(host,group_id,source_panel_id,&p,now_ms);
}
