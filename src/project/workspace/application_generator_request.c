/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/application_generator_request.c
 *
 * PURPOSE:
 *   Implement the application generator request behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Application Generator Request | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/application_generator_request.h"
#include <string.h>
/*
 * Initialise project workspace application generator request from caller-provided values
 * so later operations receive a known state.
 */
UmiStatus umi_project_workspace_application_generator_request_init(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace application generator request satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_project_workspace_application_generator_request_validate(const UmiProjectWorkspaceApplicationGeneratorRequest *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace application generator request set name operation used by
 * this module and its client applications.
 */
UmiStatus umi_project_workspace_application_generator_request_set_name(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace application generator request set detail operation used by
 * this module and its client applications.
 */
UmiStatus umi_project_workspace_application_generator_request_set_detail(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace application generator request set state operation used by
 * this module and its client applications.
 */
UmiStatus umi_project_workspace_application_generator_request_set_state(UmiProjectWorkspaceApplicationGeneratorRequest *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace application generator request set metric operation used by
 * this module and its client applications.
 */
void umi_project_workspace_application_generator_request_set_metric(UmiProjectWorkspaceApplicationGeneratorRequest *value,uint64_t metric) {
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
 * Provide the project workspace application generator request same identity operation used
 * by this module and its client applications.
 */
bool umi_project_workspace_application_generator_request_same_identity(const UmiProjectWorkspaceApplicationGeneratorRequest *left,const UmiProjectWorkspaceApplicationGeneratorRequest *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
