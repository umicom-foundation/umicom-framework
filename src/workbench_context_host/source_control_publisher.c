/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/source_control_publisher.c
 *
 * PURPOSE:
 *   Publish source-control selections as project contexts enriched with repository, branch, path and change metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/source_control_publisher.h"
#include "umicom/workbench_context_host/payload.h"
UmiStatus umi_workbench_context_host_publish_source_control(
    UmiWorkbenchContextHost *host,const char *group_id,const char *source_panel_id,
    const char *context_id,const char *project_id,const char *repository_id,
    const char *root_path,const char *branch,const char *path,const char *change_kind,
    uint64_t now_ms)
{
    UmiContextPayload p;UmiStatus s;
    if(!host||!context_id||!project_id||!repository_id||!root_path||!branch||!path||!change_kind)
        return UMI_STATUS_INVALID_ARGUMENT;
    umi_context_payload_init(&p,UMI_CONTEXT_KIND_PROJECT,context_id,"org.umicom.context.project");
    s=umi_context_copy_text(p.identity.source_application_id,sizeof(p.identity.source_application_id),host->application_id);if(s!=UMI_STATUS_OK)return s;
    if(source_panel_id){s=umi_context_copy_text(p.identity.source_panel_id,sizeof(p.identity.source_panel_id),source_panel_id);if(s!=UMI_STATUS_OK)return s;}
    s=umi_project_context_set_project_id(&p.domain.project,project_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_project_context_set_repository_id(&p.domain.project,repository_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_project_context_set_root_path(&p.domain.project,root_path);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"branch",branch);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"path",path);if(s!=UMI_STATUS_OK)return s;
    s=umi_workbench_context_host_payload_add_text(&p,"change-kind",change_kind);if(s!=UMI_STATUS_OK)return s;
    p.audit.published_at_ms=now_ms;umi_context_payload_refresh_hash(&p);
    return umi_workbench_context_host_publish(host,group_id,source_panel_id,&p,now_ms);
}
