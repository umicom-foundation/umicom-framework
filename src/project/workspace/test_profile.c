/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/test_profile.c
 *
 * PURPOSE:
 *   Implement the test profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Test Profile | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/test_profile.h"
#include <string.h>
/*
 * Initialise project workspace test profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_test_profile_init(UmiProjectWorkspaceTestProfile *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace test profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_project_workspace_test_profile_validate(const UmiProjectWorkspaceTestProfile *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace test profile set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_test_profile_set_name(UmiProjectWorkspaceTestProfile *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace test profile set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_test_profile_set_detail(UmiProjectWorkspaceTestProfile *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace test profile set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_test_profile_set_state(UmiProjectWorkspaceTestProfile *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace test profile set metric operation used by this module and
 * its client applications.
 */
void umi_project_workspace_test_profile_set_metric(UmiProjectWorkspaceTestProfile *value,uint64_t metric) {
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
 * Provide the project workspace test profile same identity operation used by this module
 * and its client applications.
 */
bool umi_project_workspace_test_profile_same_identity(const UmiProjectWorkspaceTestProfile *left,const UmiProjectWorkspaceTestProfile *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
