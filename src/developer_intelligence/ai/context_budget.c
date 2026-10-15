/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/context_budget.c
 *
 * PURPOSE:
 *   Apply the shared AI Context Budget policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/context_budget.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiContextBudget(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.context-budget",
        "AI Context Budget",
        UMI_DEVELOPER_INTELLIGENCE_BUDGET,
        0.0,
        45.0,
        0.10,
        0.20,
        0.30,
        0.40,
        0U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiContextBudgetBatch, UmiDeveloperIntelligenceEvaluateAiContextBudget,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
