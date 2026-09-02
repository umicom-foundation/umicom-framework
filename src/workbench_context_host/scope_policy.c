/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/scope_policy.c
 *
 * PURPOSE:
 *   Evaluate application and workspace constraints using canonical context audit identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/scope_policy.h"
#include <string.h>
/*
 * Initialise workbench context host scope policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_scope_policy_init(UmiWorkbenchContextHostScopePolicy *policy)
{/* Apply this branch only when its contract condition is satisfied. */ if(policy){memset(policy,0,sizeof(*policy));policy->allow_empty_workspace=true;policy->allow_cross_application=true;}}
/*
 * Provide the workbench context host scope allows operation used by this module and its
 * client applications.
 */
bool umi_workbench_context_host_scope_allows(
    const UmiWorkbenchContextHostScopePolicy *policy,const UmiContextPayload *payload)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if(!policy||!payload)return false;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(policy->require_workspace_match){
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(payload->audit.workspace_id[0]=='\0')return policy->allow_empty_workspace;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(strcmp(policy->workspace_id,payload->audit.workspace_id)!=0)return false;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(policy->require_application_match&&!policy->allow_cross_application&&
       strcmp(policy->application_id,payload->identity.source_application_id)!=0)return false;
    return true;
}
