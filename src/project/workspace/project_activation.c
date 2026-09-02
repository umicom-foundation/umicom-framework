/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_activation.c
 *
 * PURPOSE:
 *   Implement the project activation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Project Activation | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/project_activation.h"
#include <string.h>
/*
 * Initialise project workspace project activation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_project_activation_init(UmiProjectWorkspaceProjectActivation *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace project activation satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_project_activation_validate(const UmiProjectWorkspaceProjectActivation *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace project activation set name operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_activation_set_name(UmiProjectWorkspaceProjectActivation *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace project activation set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_project_activation_set_detail(UmiProjectWorkspaceProjectActivation *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace project activation set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_activation_set_state(UmiProjectWorkspaceProjectActivation *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace project activation set metric operation used by this
 * module and its client applications.
 */
void umi_project_workspace_project_activation_set_metric(UmiProjectWorkspaceProjectActivation *value,uint64_t metric) {
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
 * Provide the project workspace project activation same identity operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_project_activation_same_identity(const UmiProjectWorkspaceProjectActivation *left,const UmiProjectWorkspaceProjectActivation *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
