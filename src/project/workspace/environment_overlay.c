/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/environment_overlay.c
 *
 * PURPOSE:
 *   Implement the environment overlay behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Environment Overlay | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/environment_overlay.h"
#include <string.h>
UmiStatus umi_project_workspace_environment_overlay_init(UmiProjectWorkspaceEnvironmentOverlay *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_environment_overlay_validate(const UmiProjectWorkspaceEnvironmentOverlay *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_environment_overlay_set_name(UmiProjectWorkspaceEnvironmentOverlay *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_environment_overlay_set_detail(UmiProjectWorkspaceEnvironmentOverlay *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_environment_overlay_set_state(UmiProjectWorkspaceEnvironmentOverlay *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_environment_overlay_set_metric(UmiProjectWorkspaceEnvironmentOverlay *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_environment_overlay_same_identity(const UmiProjectWorkspaceEnvironmentOverlay *left,const UmiProjectWorkspaceEnvironmentOverlay *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
