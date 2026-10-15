/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/rag_collection_rank.c
 *
 * PURPOSE:
 *   Apply the shared RAG Collection Rank policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/rag_collection_rank.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiRagCollectionRank(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.rag-collection-rank",
        "RAG Collection Rank",
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiRagCollectionRankBatch, UmiDeveloperIntelligenceEvaluateAiRagCollectionRank,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
