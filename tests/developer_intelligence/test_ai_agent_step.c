/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_intelligence/test_ai_agent_step.c
 *
 * PURPOSE:
 *   Verify the AI Agent Step developer-intelligence policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_intelligence/ai/agent_step.h"

int main(void)
{
    UmiDeveloperIntelligenceInput input;
    UmiDeveloperIntelligenceSnapshot snapshot;

    UmiDeveloperIntelligenceInputInit(&input);
    input.lexicalScore = 88.0;
    input.semanticScore = 90.0;
    input.contextScore = 92.0;
    input.confidence = 91.0;
    input.risk = 10.0;
    input.baselineScore = 70.0;
    input.matched = 8U;
    input.total = 10U;
    input.conflicts = 0U;
    input.citations = 3U;
    input.sourceCount = 4U;
    input.usedUnits = 600U;
    input.budgetUnits = 1000U;
    input.localAvailable = 1;
    input.remoteAvailable = 1;
    input.remoteAllowed = 1;
    input.approved = 1;
    input.trusted = 1;
    input.active = 1;
    input.revision = 42U;

    assert(UmiDeveloperIntelligenceEvaluateAiAgentStep(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "ai.agent-step") == 0);
    assert(snapshot.revision == 42U);
    assert(snapshot.score >= 0.0 && snapshot.score <= 100.0);
    assert(snapshot.risk == 10.0);
    assert(snapshot.budgetRemaining == 400U);
    assert(snapshot.blocked == 0);
    assert(snapshot.ready == 1);
    input.approved = 0;
    input.risk = 95.0;
    assert(UmiDeveloperIntelligenceEvaluateAiAgentStep(&input, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.approvalRequired == 1);
    assert(snapshot.ready == 0);
    return 0;
}
