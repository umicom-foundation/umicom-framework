/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_agent_goal.c
 *
 * PURPOSE:
 *   Implement the test agent goal behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/agent_goal.h"
int main(void) {
    UmiAiDevAgentGoal v;
    umi_ai_dev_agent_goal_init(&v);
    if (umi_ai_dev_agent_goal_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_agent_goal_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_agent_goal_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
