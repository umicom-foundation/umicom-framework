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
/*
 * Initialise project workspace workspace snapshot from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_workspace_snapshot_init(UmiProjectWorkspaceWorkspaceSnapshot*v,const char*id) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(v==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(v,0,sizeof(*v));
    s=umi_pw_copy(v->workspace_id,sizeof(v->workspace_id),id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    v->revision=1U;
    v->state=UMI_PROJECT_WORKSPACE_READY;
    return UMI_STATUS_OK;
}
/*
 * Provide the project workspace workspace snapshot recalculate operation used by this
 * module and its client applications.
 */
void umi_project_workspace_workspace_snapshot_recalculate(UmiProjectWorkspaceWorkspaceSnapshot*v) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(v==NULL)return;
    /* Apply this branch only when its contract condition is satisfied. */
    if(v->blocked_project_count>0U)v->state=UMI_PROJECT_WORKSPACE_BLOCKED;
    else /* Apply this operation only while the related capability or state is available. */ if(v->ready_project_count<v->project_count)v->state=UMI_PROJECT_WORKSPACE_DEGRADED;
    /* Use this fallback path when the earlier condition does not apply. */
    else v->state=UMI_PROJECT_WORKSPACE_READY;
    v->revision+=1U;
}
