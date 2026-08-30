/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_agent_execution_policy.c
 *
 * PURPOSE:
 *   Implement the test agent execution policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/agent_execution_policy.h"
int main(void) { UmiAiDevAgentExecutionPolicy p; umi_ai_dev_agent_execution_policy_configure(&p, 7U, 4U, UMI_AI_DEV_RISK_HIGH, UMI_AI_DEV_LOCALITY_ANY, 1); if (!umi_ai_dev_agent_execution_policy_allows(&p, 1U, UMI_AI_DEV_RISK_LOW, UMI_AI_DEV_LOCALITY_LOCAL)) return 1; if (umi_ai_dev_agent_execution_policy_allows(&p, 4U, UMI_AI_DEV_RISK_LOW, UMI_AI_DEV_LOCALITY_LOCAL)) return 2; if (!umi_ai_dev_agent_execution_policy_approval_required(&p, UMI_AI_DEV_RISK_HIGH)) return 3; return 0; }
