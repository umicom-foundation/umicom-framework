/* Umicom Framework | Environment Profile | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/environment_profile.h"
#include <string.h>
UmiStatus umi_project_workspace_environment_profile_init(UmiProjectWorkspaceEnvironmentProfile *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_environment_profile_validate(const UmiProjectWorkspaceEnvironmentProfile *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_environment_profile_set_name(UmiProjectWorkspaceEnvironmentProfile *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_environment_profile_set_detail(UmiProjectWorkspaceEnvironmentProfile *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_environment_profile_set_state(UmiProjectWorkspaceEnvironmentProfile *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_environment_profile_set_metric(UmiProjectWorkspaceEnvironmentProfile *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_environment_profile_same_identity(const UmiProjectWorkspaceEnvironmentProfile *left,const UmiProjectWorkspaceEnvironmentProfile *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
