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
/*
 * Initialise context policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_policy_init(UmiContextPolicy *policy,UmiContextPolicyDecision decision){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(policy==NULL)return;
memset(policy,0,sizeof(*policy));
umi_context_policy_rule_store_init(&policy->rules);
policy->default_decision=decision;
policy->revision=1U;
}
/*
 * Provide the context policy add rule operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_policy_add_rule(UmiContextPolicy *policy,const UmiContextPolicyRule *rule){
UmiStatus s;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(policy==NULL||rule==NULL)return UMI_STATUS_INVALID_ARGUMENT;
s=umi_context_policy_rule_store_put(&policy->rules,rule);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(s==UMI_STATUS_OK)policy->revision+=1U;
return s;
}
/* Provide the match operation used by this module and its client applications. */
static bool match(const char *rule,const char *actual){
return rule[0]=='\0'||strcmp(rule,actual)==0;
}
/*
 * Provide the context policy evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_policy_evaluate(const UmiContextPolicy *policy,const UmiContextPayload *payload,const char *target,UmiContextPolicyResult *out){
size_t i;
const UmiContextPolicyRule *best=NULL;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(policy==NULL||payload==NULL||target==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
memset(out,0,sizeof(*out));
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<policy->rules.count;++i){
const UmiContextPolicyRule *r=&policy->rules.items[i];
/* Apply this operation only while the related capability or state is available. */
if(!r->enabled)continue;
/* Use the stable identifier comparison to choose the matching record or policy. */
if(!match(r->schema_id,payload->identity.schema_id)||!match(r->source_application_id,payload->identity.source_application_id)||!match(r->target_application_id,target))continue;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(best==NULL||r->priority>best->priority)best=r;
}
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(best!=NULL){
out->decision=best->decision;
(void)umi_context_copy_text(out->matched_rule_id,sizeof(out->matched_rule_id),best->rule_id);
(void)umi_context_copy_text(out->reason,sizeof(out->reason),"matched explicit context policy");
} /* Use this fallback path when the earlier condition does not apply. */ else{
out->decision=policy->default_decision;
(void)umi_context_copy_text(out->reason,sizeof(out->reason),"default context policy");
}
return UMI_STATUS_OK;
}
