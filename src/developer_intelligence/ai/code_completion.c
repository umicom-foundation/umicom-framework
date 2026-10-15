/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/code_completion.c
 *
 * PURPOSE:
 *   Apply the shared AI Code Completion policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/code_completion.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiCodeCompletion(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.code-completion",
        "AI Code Completion",
        UMI_DEVELOPER_INTELLIGENCE_APPROVAL,
        65.0,
        25.0,
        0.15,
        0.30,
        0.35,
        0.20,
        1U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiCodeCompletionBatch, UmiDeveloperIntelligenceEvaluateAiCodeCompletion,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
