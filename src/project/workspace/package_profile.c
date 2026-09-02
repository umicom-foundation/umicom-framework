/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/package_profile.c
 *
 * PURPOSE:
 *   Implement the package profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Package Profile | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/package_profile.h"
#include <string.h>
/*
 * Initialise project workspace package profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_package_profile_init(UmiProjectWorkspacePackageProfile *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace package profile satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_package_profile_validate(const UmiProjectWorkspacePackageProfile *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace package profile set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_package_profile_set_name(UmiProjectWorkspacePackageProfile *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace package profile set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_package_profile_set_detail(UmiProjectWorkspacePackageProfile *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace package profile set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_package_profile_set_state(UmiProjectWorkspacePackageProfile *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace package profile set metric operation used by this module
 * and its client applications.
 */
void umi_project_workspace_package_profile_set_metric(UmiProjectWorkspacePackageProfile *value,uint64_t metric) {
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
 * Provide the project workspace package profile same identity operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_package_profile_same_identity(const UmiProjectWorkspacePackageProfile *left,const UmiProjectWorkspacePackageProfile *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
