/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/rag_source_rank.c
 *
 * PURPOSE:
 *   Apply the shared RAG Source Rank policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/rag_source_rank.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiRagSourceRank(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.rag-source-rank",
        "RAG Source Rank",
        UMI_DEVELOPER_INTELLIGENCE_RETRIEVAL,
        60.0,
        40.0,
        0.20,
        0.35,
        0.30,
        0.15,
        1U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiRagSourceRankBatch, UmiDeveloperIntelligenceEvaluateAiRagSourceRank,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
