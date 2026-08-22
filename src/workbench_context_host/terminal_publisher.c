/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/terminal_publisher.c
 *
 * PURPOSE:
 *   Publish terminal session selection and working-directory metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/terminal_publisher.h"
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_host/payload.h"
UmiStatus umi_workbench_context_host_publish_terminal(
    UmiWorkbenchContextHost *host,const char *group_id,const char *source_panel_id,
    const char *context_id,const char *session_id,const char *shell_id,
    const char *working_directory,const char *command_text,int64_t exit_code,uint64_t now_ms)
{
    UmiContextPayload p;UmiContextValue v;UmiStatus s;
    if(!host||!context_id||!session_id||!shell_id||!working_directory||!command_text)
        return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_link_build_selection(&p,context_id,host->application_id,
        source_panel_id?source_panel_id:"terminal",session_id,"terminal-session",session_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"shell-id",shell_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"working-directory",working_directory);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"command",command_text);if(s!=UMI_STATUS_OK)return s;
    umi_context_value_init(&v,"exit-code");s=umi_context_value_set_integer(&v,exit_code);if(s!=UMI_STATUS_OK)return s;
    s=umi_context_payload_add_value(&p,&v);if(s!=UMI_STATUS_OK)return s;
    p.audit.published_at_ms=now_ms;umi_context_payload_refresh_hash(&p);
    return umi_workbench_context_host_publish(host,group_id,source_panel_id,&p,now_ms);
}
