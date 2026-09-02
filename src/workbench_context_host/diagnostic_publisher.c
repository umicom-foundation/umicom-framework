/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/diagnostic_publisher.c
 *
 * PURPOSE:
 *   Publish source diagnostics as source-location contexts enriched with severity, code and message metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/diagnostic_publisher.h"
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_host/payload.h"
/*
 * Provide the workbench context host publish diagnostic operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_publish_diagnostic(
    UmiWorkbenchContextHost *host,const char *group_id,const char *source_panel_id,
    const char *context_id,const char *workspace_id,const char *file_path,
    uint32_t line,uint32_t column,const char *severity,const char *diagnostic_code,
    const char *message,uint64_t now_ms)
{
    UmiContextPayload p;UmiStatus s;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!host||!context_id||!workspace_id||!file_path||!severity||!diagnostic_code||!message)
        return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_link_build_source_location(&p,context_id,host->application_id,
        source_panel_id?source_panel_id:"diagnostics",workspace_id,file_path,line,column);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"severity",severity);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"diagnostic-code",diagnostic_code);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"message",message);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    p.audit.published_at_ms=now_ms;umi_context_payload_refresh_hash(&p);
    return umi_workbench_context_host_publish(host,group_id,source_panel_id,&p,now_ms);
}
