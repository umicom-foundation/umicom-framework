/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_resource_binding.c
 *
 * PURPOSE:
 *   Implement the workspace resource binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Resource Binding | MIT */
#include "umicom/project/workspace/workspace_resource_binding.h"
#include "internal.h"
#include <string.h>
/*
 * Initialise project workspace workspace resource binding from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_project_workspace_workspace_resource_binding_init(UmiProjectWorkspaceWorkspaceResourceBinding *value,const char *owner_id,const char *target_id) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||owner_id==NULL||target_id==NULL||owner_id[0]=='\0'||target_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    s=umi_pw_copy(value->owner_id,sizeof(value->owner_id),owner_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->target_id,sizeof(value->target_id),target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    value->revision=1U;
    value->enabled=true;
    return UMI_STATUS_OK;
}
/*
 * Provide the project workspace workspace resource binding retarget operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_workspace_resource_binding_retarget(UmiProjectWorkspaceWorkspaceResourceBinding *value,const char *target_id) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_pw_copy(value->target_id,sizeof(value->target_id),target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
}
/*
 * Provide the project workspace workspace resource binding matches operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_workspace_resource_binding_matches(const UmiProjectWorkspaceWorkspaceResourceBinding *value,const char *owner_id,const char *target_id) {
    return value!=NULL&&owner_id!=NULL&&target_id!=NULL&&strcmp(value->owner_id,owner_id)==0&&strcmp(value->target_id,target_id)==0;
}
