/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/code_action_rank.c
 *
 * PURPOSE:
 *   Apply the shared Code Action Rank policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/code_action_rank.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCodeActionRank(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.code-action-rank",
        "Code Action Rank",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        60.0,
        45.0,
        0.20,
        0.30,
        0.30,
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageCodeActionRankBatch, UmiDeveloperIntelligenceEvaluateLanguageCodeActionRank,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
