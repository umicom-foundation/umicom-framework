/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/symbol_rank.c
 *
 * PURPOSE:
 *   Apply the shared Workspace Symbol Rank policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/symbol_rank.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageSymbolRank(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.symbol-rank",
        "Workspace Symbol Rank",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        60.0,
        55.0,
        0.35,
        0.25,
        0.20,
        0.20,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
