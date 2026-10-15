/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/semantic_token_delta.c
 *
 * PURPOSE:
 *   Apply the shared Semantic Token Delta policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/semantic_token_delta.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageSemanticTokenDelta(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.semantic-token-delta",
        "Semantic Token Delta",
        UMI_DEVELOPER_INTELLIGENCE_FRESHNESS,
        65.0,
        50.0,
        0.05,
        0.35,
        0.40,
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageSemanticTokenDeltaBatch, UmiDeveloperIntelligenceEvaluateLanguageSemanticTokenDelta,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
