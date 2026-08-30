/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/cmake_preset.c
 *
 * PURPOSE:
 *   Implement the cmake preset behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Cmake Preset | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/cmake_preset.h"
#include <string.h>
UmiStatus umi_project_workspace_cmake_preset_init(UmiProjectWorkspaceCmakePreset *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_cmake_preset_validate(const UmiProjectWorkspaceCmakePreset *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_cmake_preset_set_name(UmiProjectWorkspaceCmakePreset *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_cmake_preset_set_detail(UmiProjectWorkspaceCmakePreset *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_cmake_preset_set_state(UmiProjectWorkspaceCmakePreset *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_cmake_preset_set_metric(UmiProjectWorkspaceCmakePreset *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_cmake_preset_same_identity(const UmiProjectWorkspaceCmakePreset *left,const UmiProjectWorkspaceCmakePreset *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
