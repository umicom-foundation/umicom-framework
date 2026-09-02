/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/policy.h
 *
 * PURPOSE:
 *   Define AI safety policy for remote providers, tools and human approval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Policy is checked before invoking a provider or tool, so a model cannot grant itself access by returning a persuasive text response.
 */

#ifndef INCLUDE_UMICOM_AI_POLICY_H
#define INCLUDE_UMICOM_AI_POLICY_H

#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai policy data shared with callers of this public contract.
 */
typedef struct UmiAiPolicy {
    int allow_local;
    int allow_remote;
    int allow_tools;
    int require_tool_approval;
    int require_remote_approval;
} UmiAiPolicy;

/**
 * Provide the ai policy default operation used by this module and its client applications.
 */
UmiAiPolicy umi_ai_policy_default(void);
/**
 * Provide the ai policy check provider operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_policy_check_provider(const UmiAiPolicy *policy,
                                       UmiAiProviderKind kind,
                                       int approved);
/**
 * Provide the ai policy check tool operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_policy_check_tool(const UmiAiPolicy *policy, int approved);

#ifdef __cplusplus
}
#endif

#endif
