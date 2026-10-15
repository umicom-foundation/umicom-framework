/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/rerank_score.c
 *
 * PURPOSE:
 *   Apply the shared RAG Rerank policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/rerank_score.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiRerankScore(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.rerank-score",
        "RAG Rerank",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        65.0,
        40.0,
        0.20,
        0.40,
        0.25,
        0.15,
        2U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiRerankScoreBatch, UmiDeveloperIntelligenceEvaluateAiRerankScore,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
