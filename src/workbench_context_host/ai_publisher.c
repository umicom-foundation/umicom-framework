/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/ai_publisher.c
 *
 * PURPOSE:
 *   Publish AI conversation selections while preserving provider, conversation and evidence identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/ai_publisher.h"
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_host/payload.h"
UmiStatus umi_workbench_context_host_publish_ai(
    UmiWorkbenchContextHost *host,const char *group_id,const char *source_panel_id,
    const char *context_id,const char *conversation_id,const char *message_id,
    const char *provider_id,const char *model_id,const char *evidence_id,uint64_t now_ms)
{
    UmiContextPayload p;UmiStatus s;
    if(!host||!context_id||!conversation_id||!message_id||!provider_id||!model_id||!evidence_id)
        return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_link_build_selection(&p,context_id,host->application_id,
        source_panel_id?source_panel_id:"ai",message_id,"ai-message",message_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"conversation-id",conversation_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"provider-id",provider_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"model-id",model_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"evidence-id",evidence_id);if(s!=UMI_STATUS_OK)return s;
    p.audit.published_at_ms=now_ms;umi_context_payload_refresh_hash(&p);
    return umi_workbench_context_host_publish(host,group_id,source_panel_id,&p,now_ms);
}
