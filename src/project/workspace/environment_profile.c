/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/environment_profile.c
 *
 * PURPOSE:
 *   Implement the environment profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Environment Profile | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/environment_profile.h"
#include <string.h>
/*
 * Initialise project workspace environment profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_environment_profile_init(UmiProjectWorkspaceEnvironmentProfile *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace environment profile satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_environment_profile_validate(const UmiProjectWorkspaceEnvironmentProfile *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace environment profile set name operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_environment_profile_set_name(UmiProjectWorkspaceEnvironmentProfile *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace environment profile set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_environment_profile_set_detail(UmiProjectWorkspaceEnvironmentProfile *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace environment profile set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_environment_profile_set_state(UmiProjectWorkspaceEnvironmentProfile *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace environment profile set metric operation used by this
 * module and its client applications.
 */
void umi_project_workspace_environment_profile_set_metric(UmiProjectWorkspaceEnvironmentProfile *value,uint64_t metric) {
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
 * Provide the project workspace environment profile same identity operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_environment_profile_same_identity(const UmiProjectWorkspaceEnvironmentProfile *left,const UmiProjectWorkspaceEnvironmentProfile *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
