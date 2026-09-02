/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/agent_execution_policy.h
 *
 * PURPOSE:
 *   Apply read/write/network/process permission gates before tool execution.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AGENT_EXECUTION_POLICY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AGENT_EXECUTION_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev agent execution policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevAgentExecutionPolicy {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevAgentExecutionPolicy;

/**
 * Initialise ai dev agent execution policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_agent_execution_policy_init(UmiAiDevAgentExecutionPolicy *policy);
/**
 * Provide the ai dev agent execution policy configure operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_dev_agent_execution_policy_configure(UmiAiDevAgentExecutionPolicy *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev agent execution policy allows operation used by this module and its
 * client applications.
 */
int umi_ai_dev_agent_execution_policy_allows(const UmiAiDevAgentExecutionPolicy *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev agent execution policy approval required operation used by this
 * module and its client applications.
 */
int umi_ai_dev_agent_execution_policy_approval_required(const UmiAiDevAgentExecutionPolicy *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
