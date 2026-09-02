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
/*
 * Initialise project workspace cmake configure model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_cmake_configure_model_init(UmiProjectWorkspaceCmakeConfigureModel *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace cmake configure model satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_cmake_configure_model_validate(const UmiProjectWorkspaceCmakeConfigureModel *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace cmake configure model set name operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_cmake_configure_model_set_name(UmiProjectWorkspaceCmakeConfigureModel *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace cmake configure model set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_cmake_configure_model_set_detail(UmiProjectWorkspaceCmakeConfigureModel *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace cmake configure model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_cmake_configure_model_set_state(UmiProjectWorkspaceCmakeConfigureModel *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace cmake configure model set metric operation used by this
 * module and its client applications.
 */
void umi_project_workspace_cmake_configure_model_set_metric(UmiProjectWorkspaceCmakeConfigureModel *value,uint64_t metric) {
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
 * Provide the project workspace cmake configure model same identity operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_cmake_configure_model_same_identity(const UmiProjectWorkspaceCmakeConfigureModel *left,const UmiProjectWorkspaceCmakeConfigureModel *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
