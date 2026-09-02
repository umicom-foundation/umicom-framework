/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_policy.c
 *
 * PURPOSE:
 *   Implement the workspace policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Policy | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/workspace_policy.h"
#include <string.h>
/*
 * Initialise project workspace workspace policy from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_workspace_policy_init(UmiProjectWorkspaceWorkspacePolicy *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace workspace policy satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_workspace_policy_validate(const UmiProjectWorkspaceWorkspacePolicy *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace workspace policy set name operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_workspace_policy_set_name(UmiProjectWorkspaceWorkspacePolicy *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace workspace policy set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_workspace_policy_set_detail(UmiProjectWorkspaceWorkspacePolicy *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace workspace policy set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_workspace_policy_set_state(UmiProjectWorkspaceWorkspacePolicy *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace workspace policy set metric operation used by this module
 * and its client applications.
 */
void umi_project_workspace_workspace_policy_set_metric(UmiProjectWorkspaceWorkspacePolicy *value,uint64_t metric) {
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
 * Provide the project workspace workspace policy same identity operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_workspace_policy_same_identity(const UmiProjectWorkspaceWorkspacePolicy *left,const UmiProjectWorkspaceWorkspacePolicy *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
