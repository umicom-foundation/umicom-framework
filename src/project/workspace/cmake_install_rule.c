/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/cmake_install_rule.c
 *
 * PURPOSE:
 *   Implement the cmake install rule behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Cmake Install Rule | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/cmake_install_rule.h"
#include <string.h>
UmiStatus umi_project_workspace_cmake_install_rule_init(UmiProjectWorkspaceCmakeInstallRule *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_cmake_install_rule_validate(const UmiProjectWorkspaceCmakeInstallRule *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_cmake_install_rule_set_name(UmiProjectWorkspaceCmakeInstallRule *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_cmake_install_rule_set_detail(UmiProjectWorkspaceCmakeInstallRule *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_cmake_install_rule_set_state(UmiProjectWorkspaceCmakeInstallRule *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_cmake_install_rule_set_metric(UmiProjectWorkspaceCmakeInstallRule *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_cmake_install_rule_same_identity(const UmiProjectWorkspaceCmakeInstallRule *left,const UmiProjectWorkspaceCmakeInstallRule *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
