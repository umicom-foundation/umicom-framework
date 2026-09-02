/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/repository_bootstrap_plan.c
 *
 * PURPOSE:
 *   Implement the repository bootstrap plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Repository Bootstrap Plan | MIT */
#include "umicom/project/workspace/repository_bootstrap_plan.h"
#include "internal.h"
#include <string.h>
/*
 * Initialise project workspace repository bootstrap plan from caller-provided values so
 * later operations receive a known state.
 */
void umi_project_workspace_repository_bootstrap_plan_init(UmiProjectWorkspaceRepositoryBootstrapPlan *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value!=NULL) {
        (void)memset(value,0,sizeof(*value));
        value->revision=1U;
    }
}
/*
 * Add project workspace repository bootstrap plan only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_project_workspace_repository_bootstrap_plan_add(UmiProjectWorkspaceRepositoryBootstrapPlan *value,const char *id,const char *action,bool required) {
    UmiProjectWorkspaceRepositoryBootstrapPlanStep *s;
    size_t i;
    UmiStatus st;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||id==NULL||action==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(value->count>=UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<value->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(value->steps[i].id,id)==0)return UMI_STATUS_ALREADY_EXISTS;
    s=&value->steps[value->count];
    (void)memset(s,0,sizeof(*s));
    st=umi_pw_copy(s->id,sizeof(s->id),id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(st!=UMI_STATUS_OK)return st;
    st=umi_pw_copy(s->action,sizeof(s->action),action);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(st!=UMI_STATUS_OK)return st;
    s->required=required;
    value->count+=1U;
    value->revision+=1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the project workspace repository bootstrap plan complete operation used by this
 * module and its client applications.
 */
UmiStatus umi_project_workspace_repository_bootstrap_plan_complete(UmiProjectWorkspaceRepositoryBootstrapPlan *value,const char *id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<value->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(value->steps[i].id,id)==0) {
        /* Apply this branch only when its contract condition is satisfied. */
        if(!value->steps[i].completed) {
            value->steps[i].completed=true;
            value->completed_count+=1U;
            value->revision+=1U;
        }
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Provide the project workspace repository bootstrap plan ready operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_repository_bootstrap_plan_ready(const UmiProjectWorkspaceRepositoryBootstrapPlan *value) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<value->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->steps[i].required&&!value->steps[i].completed)return false;
    return true;
}
