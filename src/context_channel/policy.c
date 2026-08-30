/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/policy.c
 *
 * PURPOSE:
 *   Evaluate cross-application context-sharing policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/policy.h"
#include <string.h>
void umi_context_policy_init(UmiContextPolicy *policy,UmiContextPolicyDecision decision){
if(policy==NULL)return;
memset(policy,0,sizeof(*policy));
umi_context_policy_rule_store_init(&policy->rules);
policy->default_decision=decision;
policy->revision=1U;
}
UmiStatus umi_context_policy_add_rule(UmiContextPolicy *policy,const UmiContextPolicyRule *rule){
UmiStatus s;
if(policy==NULL||rule==NULL)return UMI_STATUS_INVALID_ARGUMENT;
s=umi_context_policy_rule_store_put(&policy->rules,rule);
if(s==UMI_STATUS_OK)policy->revision+=1U;
return s;
}
static bool match(const char *rule,const char *actual){
return rule[0]=='\0'||strcmp(rule,actual)==0;
}
UmiStatus umi_context_policy_evaluate(const UmiContextPolicy *policy,const UmiContextPayload *payload,const char *target,UmiContextPolicyResult *out){
size_t i;
const UmiContextPolicyRule *best=NULL;
if(policy==NULL||payload==NULL||target==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
memset(out,0,sizeof(*out));
for(i=0U;i<policy->rules.count;++i){
const UmiContextPolicyRule *r=&policy->rules.items[i];
if(!r->enabled)continue;
if(!match(r->schema_id,payload->identity.schema_id)||!match(r->source_application_id,payload->identity.source_application_id)||!match(r->target_application_id,target))continue;
if(best==NULL||r->priority>best->priority)best=r;
}
if(best!=NULL){
out->decision=best->decision;
(void)umi_context_copy_text(out->matched_rule_id,sizeof(out->matched_rule_id),best->rule_id);
(void)umi_context_copy_text(out->reason,sizeof(out->reason),"matched explicit context policy");
} else{
out->decision=policy->default_decision;
(void)umi_context_copy_text(out->reason,sizeof(out->reason),"default context policy");
}
return UMI_STATUS_OK;
}
