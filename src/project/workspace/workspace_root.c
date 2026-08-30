/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_root.c
 *
 * PURPOSE:
 *   Implement the workspace root behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Root | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/workspace_root.h"
#include <string.h>
UmiStatus umi_project_workspace_workspace_root_init(UmiProjectWorkspaceWorkspaceRoot *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_workspace_root_validate(const UmiProjectWorkspaceWorkspaceRoot *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_workspace_root_set_name(UmiProjectWorkspaceWorkspaceRoot *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_workspace_root_set_detail(UmiProjectWorkspaceWorkspaceRoot *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_workspace_root_set_state(UmiProjectWorkspaceWorkspaceRoot *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_workspace_root_set_metric(UmiProjectWorkspaceWorkspaceRoot *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_workspace_root_same_identity(const UmiProjectWorkspaceWorkspaceRoot *left,const UmiProjectWorkspaceWorkspaceRoot *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
