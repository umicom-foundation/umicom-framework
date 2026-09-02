/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_root.c
 *
 * PURPOSE:
 *   Implement the workspace root behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Root | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/workspace_root.h"
#include <string.h>
/*
 * Initialise project workspace workspace root from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_workspace_root_init(UmiProjectWorkspaceWorkspaceRoot *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace workspace root satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_workspace_root_validate(const UmiProjectWorkspaceWorkspaceRoot *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace workspace root set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_workspace_root_set_name(UmiProjectWorkspaceWorkspaceRoot *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace workspace root set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_workspace_root_set_detail(UmiProjectWorkspaceWorkspaceRoot *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace workspace root set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_workspace_root_set_state(UmiProjectWorkspaceWorkspaceRoot *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace workspace root set metric operation used by this module
 * and its client applications.
 */
void umi_project_workspace_workspace_root_set_metric(UmiProjectWorkspaceWorkspaceRoot *value,uint64_t metric) {
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
 * Provide the project workspace workspace root same identity operation used by this module
 * and its client applications.
 */
bool umi_project_workspace_workspace_root_same_identity(const UmiProjectWorkspaceWorkspaceRoot *left,const UmiProjectWorkspaceWorkspaceRoot *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
