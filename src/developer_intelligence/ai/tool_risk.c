/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/tool_risk.c
 *
 * PURPOSE:
 *   Apply the shared AI Tool Risk policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/developer_intelligence/ai/tool_risk.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiToolRisk(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.tool-risk",
        "AI Tool Risk",
        UMI_DEVELOPER_INTELLIGENCE_RISK,
        60.0,
        20.0,
        0.10,
        0.25,
        0.30,
        0.35,
        0U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiToolRiskBatch, UmiDeveloperIntelligenceEvaluateAiToolRisk,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
