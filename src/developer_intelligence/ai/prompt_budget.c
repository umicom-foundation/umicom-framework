/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/prompt_budget.c
 *
 * PURPOSE:
 *   Apply the shared Prompt Budget policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/prompt_budget.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiPromptBudget(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.prompt-budget",
        "Prompt Budget",
        UMI_DEVELOPER_INTELLIGENCE_BUDGET,
        0.0,
        40.0,
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiPromptBudgetBatch, UmiDeveloperIntelligenceEvaluateAiPromptBudget,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
