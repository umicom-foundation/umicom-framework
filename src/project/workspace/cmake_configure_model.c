/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/cmake_configure_model.c
 *
 * PURPOSE:
 *   Implement the cmake configure model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Cmake Configure Model | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/cmake_configure_model.h"
#include <string.h>
UmiStatus umi_project_workspace_cmake_configure_model_init(UmiProjectWorkspaceCmakeConfigureModel *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_cmake_configure_model_validate(const UmiProjectWorkspaceCmakeConfigureModel *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_cmake_configure_model_set_name(UmiProjectWorkspaceCmakeConfigureModel *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_cmake_configure_model_set_detail(UmiProjectWorkspaceCmakeConfigureModel *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_cmake_configure_model_set_state(UmiProjectWorkspaceCmakeConfigureModel *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_cmake_configure_model_set_metric(UmiProjectWorkspaceCmakeConfigureModel *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_cmake_configure_model_same_identity(const UmiProjectWorkspaceCmakeConfigureModel *left,const UmiProjectWorkspaceCmakeConfigureModel *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
