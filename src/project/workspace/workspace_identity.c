/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_identity.c
 *
 * PURPOSE:
 *   Implement the workspace identity behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Identity | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/workspace_identity.h"
#include <string.h>
UmiStatus umi_project_workspace_workspace_identity_init(UmiProjectWorkspaceWorkspaceIdentity *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_workspace_identity_validate(const UmiProjectWorkspaceWorkspaceIdentity *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_workspace_identity_set_name(UmiProjectWorkspaceWorkspaceIdentity *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_workspace_identity_set_detail(UmiProjectWorkspaceWorkspaceIdentity *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_workspace_identity_set_state(UmiProjectWorkspaceWorkspaceIdentity *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_workspace_identity_set_metric(UmiProjectWorkspaceWorkspaceIdentity *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_workspace_identity_same_identity(const UmiProjectWorkspaceWorkspaceIdentity *left,const UmiProjectWorkspaceWorkspaceIdentity *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
