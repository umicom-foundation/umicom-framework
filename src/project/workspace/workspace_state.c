/* Umicom Framework | Workspace State | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/workspace_state.h"
#include <string.h>
UmiStatus umi_project_workspace_workspace_state_init(UmiProjectWorkspaceWorkspaceState *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_workspace_state_validate(const UmiProjectWorkspaceWorkspaceState *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_workspace_state_set_name(UmiProjectWorkspaceWorkspaceState *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_workspace_state_set_detail(UmiProjectWorkspaceWorkspaceState *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_workspace_state_set_state(UmiProjectWorkspaceWorkspaceState *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_workspace_state_set_metric(UmiProjectWorkspaceWorkspaceState *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_workspace_state_same_identity(const UmiProjectWorkspaceWorkspaceState *left,const UmiProjectWorkspaceWorkspaceState *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
