/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_json.c
 *
 * PURPOSE:
 *   Implement the workspace json behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_json.h"
#include <stdio.h>
/*
 * Provide the project workspace workspace json render operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_workspace_json_render(const UmiProjectWorkspaceWorkspaceSnapshot*v,char*out_json,size_t capacity) {
    int n;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(v==NULL||out_json==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;
    n=snprintf(out_json,capacity,"{\"workspaceId\":\"%s\",\"roots\":%zu,\"projects\":%zu,\"state\":\"%s\",\"revision\":%llu}",v->workspace_id,v->root_count,v->project_count,umi_project_workspace_state_text(v->state),(unsigned long long)v->revision);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(n<0||(size_t)n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
