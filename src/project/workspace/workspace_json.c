#include "umicom/project/workspace/workspace_json.h"
#include <stdio.h>
UmiStatus umi_project_workspace_workspace_json_render(const UmiProjectWorkspaceWorkspaceSnapshot*v,char*out_json,size_t capacity) {
    int n;
    if(v==NULL||out_json==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;
    n=snprintf(out_json,capacity,"{\"workspaceId\":\"%s\",\"roots\":%zu,\"projects\":%zu,\"state\":\"%s\",\"revision\":%llu}",v->workspace_id,v->root_count,v->project_count,umi_project_workspace_state_text(v->state),(unsigned long long)v->revision);
    if(n<0||(size_t)n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
