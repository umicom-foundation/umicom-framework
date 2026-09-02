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
/*
 * Initialise project workspace cmake preset from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_cmake_preset_init(UmiProjectWorkspaceCmakePreset *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace cmake preset satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_project_workspace_cmake_preset_validate(const UmiProjectWorkspaceCmakePreset *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace cmake preset set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_cmake_preset_set_name(UmiProjectWorkspaceCmakePreset *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace cmake preset set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_cmake_preset_set_detail(UmiProjectWorkspaceCmakePreset *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace cmake preset set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_cmake_preset_set_state(UmiProjectWorkspaceCmakePreset *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace cmake preset set metric operation used by this module and
 * its client applications.
 */
void umi_project_workspace_cmake_preset_set_metric(UmiProjectWorkspaceCmakePreset *value,uint64_t metric) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
/*
 * Provide the project workspace cmake preset same identity operation used by this module
 * and its client applications.
 */
bool umi_project_workspace_cmake_preset_same_identity(const UmiProjectWorkspaceCmakePreset *left,const UmiProjectWorkspaceCmakePreset *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
