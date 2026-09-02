/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_capability.c
 *
 * PURPOSE:
 *   Implement the project capability behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Project Capability | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/project_capability.h"
#include <string.h>
/*
 * Initialise project workspace project capability from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_project_capability_init(UmiProjectWorkspaceProjectCapability *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace project capability satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_project_capability_validate(const UmiProjectWorkspaceProjectCapability *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace project capability set name operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_capability_set_name(UmiProjectWorkspaceProjectCapability *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace project capability set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_project_capability_set_detail(UmiProjectWorkspaceProjectCapability *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace project capability set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_capability_set_state(UmiProjectWorkspaceProjectCapability *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace project capability set metric operation used by this
 * module and its client applications.
 */
void umi_project_workspace_project_capability_set_metric(UmiProjectWorkspaceProjectCapability *value,uint64_t metric) {
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
 * Provide the project workspace project capability same identity operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_project_capability_same_identity(const UmiProjectWorkspaceProjectCapability *left,const UmiProjectWorkspaceProjectCapability *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
