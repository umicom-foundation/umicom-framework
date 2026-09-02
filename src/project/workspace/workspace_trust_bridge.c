/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_trust_bridge.c
 *
 * PURPOSE:
 *   Implement the workspace trust bridge behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Trust Bridge | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/workspace_trust_bridge.h"
#include <string.h>
/*
 * Initialise project workspace workspace trust bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_workspace_trust_bridge_init(UmiProjectWorkspaceWorkspaceTrustBridge *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace workspace trust bridge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_project_workspace_workspace_trust_bridge_validate(const UmiProjectWorkspaceWorkspaceTrustBridge *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace workspace trust bridge set name operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_workspace_trust_bridge_set_name(UmiProjectWorkspaceWorkspaceTrustBridge *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace workspace trust bridge set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_workspace_trust_bridge_set_detail(UmiProjectWorkspaceWorkspaceTrustBridge *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace workspace trust bridge set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_workspace_trust_bridge_set_state(UmiProjectWorkspaceWorkspaceTrustBridge *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace workspace trust bridge set metric operation used by this
 * module and its client applications.
 */
void umi_project_workspace_workspace_trust_bridge_set_metric(UmiProjectWorkspaceWorkspaceTrustBridge *value,uint64_t metric) {
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
 * Provide the project workspace workspace trust bridge same identity operation used by
 * this module and its client applications.
 */
bool umi_project_workspace_workspace_trust_bridge_same_identity(const UmiProjectWorkspaceWorkspaceTrustBridge *left,const UmiProjectWorkspaceWorkspaceTrustBridge *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
