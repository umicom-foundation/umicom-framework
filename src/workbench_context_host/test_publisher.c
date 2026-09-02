/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/test_publisher.c
 *
 * PURPOSE:
 *   Publish Test Explorer selections with stable suite, outcome and source metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/test_publisher.h"
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_host/payload.h"
/*
 * Provide the workbench context host publish test operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_publish_test(
    UmiWorkbenchContextHost *host,const char *group_id,const char *source_panel_id,
    const char *context_id,const char *test_id,const char *suite_id,const char *outcome,
    const char *source_uri,uint64_t source_line,uint64_t duration_ms,uint64_t now_ms)
{
    UmiContextPayload p;UmiStatus s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!host||!context_id||!test_id||!suite_id||!outcome||!source_uri)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_link_build_selection(&p,context_id,host->application_id,
        source_panel_id?source_panel_id:"test-explorer",test_id,"test",test_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"suite-id",suite_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"outcome",outcome);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"source-uri",source_uri);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_unsigned(&p,"source-line",source_line);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_unsigned(&p,"duration-ms",duration_ms);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    p.audit.published_at_ms=now_ms;umi_context_payload_refresh_hash(&p);
    return umi_workbench_context_host_publish(host,group_id,source_panel_id,&p,now_ms);
}
