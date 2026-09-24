/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/inlay_hint_rank.c
 *
 * PURPOSE:
 *   Apply the shared Inlay Hint Rank policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/inlay_hint_rank.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageInlayHintRank(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.inlay-hint-rank",
        "Inlay Hint Rank",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        55.0,
        55.0,
        0.15,
        0.35,
        0.35,
        0.15,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
