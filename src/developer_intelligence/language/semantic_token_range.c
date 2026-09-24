/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/semantic_token_range.c
 *
 * PURPOSE:
 *   Apply the shared Semantic Token Range policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/semantic_token_range.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageSemanticTokenRange(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.semantic-token-range",
        "Semantic Token Range",
        UMI_DEVELOPER_INTELLIGENCE_FRESHNESS,
        60.0,
        50.0,
        0.05,
        0.40,
        0.35,
        0.20,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
