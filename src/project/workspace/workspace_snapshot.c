/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_snapshot.c
 *
 * PURPOSE:
 *   Implement the workspace snapshot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_snapshot.h"
#include "internal.h"
#include <string.h>
UmiStatus umi_project_workspace_workspace_snapshot_init(UmiProjectWorkspaceWorkspaceSnapshot*v,const char*id) {
    UmiStatus s;
    if(v==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(v,0,sizeof(*v));
    s=umi_pw_copy(v->workspace_id,sizeof(v->workspace_id),id);
    if(s!=UMI_STATUS_OK)return s;
    v->revision=1U;
    v->state=UMI_PROJECT_WORKSPACE_READY;
    return UMI_STATUS_OK;
}
void umi_project_workspace_workspace_snapshot_recalculate(UmiProjectWorkspaceWorkspaceSnapshot*v) {
    if(v==NULL)return;
    if(v->blocked_project_count>0U)v->state=UMI_PROJECT_WORKSPACE_BLOCKED;
    else if(v->ready_project_count<v->project_count)v->state=UMI_PROJECT_WORKSPACE_DEGRADED;
    else v->state=UMI_PROJECT_WORKSPACE_READY;
    v->revision+=1U;
}
