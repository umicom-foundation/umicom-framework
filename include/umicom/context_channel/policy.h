/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/policy.h
 *
 * PURPOSE:
 *   Evaluate cross-application data-sharing policy before any subscriber receives a context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_POLICY_H
#define UMICOM_CONTEXT_CHANNEL_POLICY_H
#include "umicom/context_channel/policy_rule.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextPolicy { UmiContextPolicyRuleStore rules; UmiContextPolicyDecision default_decision; uint64_t revision; } UmiContextPolicy;
typedef struct UmiContextPolicyResult { UmiContextPolicyDecision decision; char matched_rule_id[UMI_CONTEXT_TEXT_CAPACITY]; char reason[UMI_CONTEXT_VALUE_CAPACITY]; } UmiContextPolicyResult;
void umi_context_policy_init(UmiContextPolicy *policy,UmiContextPolicyDecision default_decision);
UmiStatus umi_context_policy_add_rule(UmiContextPolicy *policy,const UmiContextPolicyRule *rule);
UmiStatus umi_context_policy_evaluate(const UmiContextPolicy *policy,const UmiContextPayload *payload,const char *target_application_id,UmiContextPolicyResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
