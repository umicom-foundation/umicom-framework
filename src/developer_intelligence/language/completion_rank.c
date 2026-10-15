/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/completion_rank.c
 *
 * PURPOSE:
 *   Apply the shared Completion Rank policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/completion_rank.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCompletionRank(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.completion-rank",
        "Completion Rank",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        65.0,
        55.0,
        0.30,
        0.25,
        0.25,
        0.20,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageCompletionRankBatch, UmiDeveloperIntelligenceEvaluateLanguageCompletionRank,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
