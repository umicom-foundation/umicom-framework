/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/scope_policy.c
 *
 * PURPOSE:
 *   Evaluate application and workspace constraints using canonical context audit identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/scope_policy.h"
#include <string.h>
void umi_workbench_context_host_scope_policy_init(UmiWorkbenchContextHostScopePolicy *policy)
{if(policy){memset(policy,0,sizeof(*policy));policy->allow_empty_workspace=true;policy->allow_cross_application=true;}}
bool umi_workbench_context_host_scope_allows(
    const UmiWorkbenchContextHostScopePolicy *policy,const UmiContextPayload *payload)
{
    if(!policy||!payload)return false;
    if(policy->require_workspace_match){
        if(payload->audit.workspace_id[0]=='\0')return policy->allow_empty_workspace;
        if(strcmp(policy->workspace_id,payload->audit.workspace_id)!=0)return false;
    }
    if(policy->require_application_match&&!policy->allow_cross_application&&
       strcmp(policy->application_id,payload->identity.source_application_id)!=0)return false;
    return true;
}
