/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/reference_rank.c
 *
 * PURPOSE:
 *   Apply the shared Reference Rank policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/reference_rank.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageReferenceRank(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.reference-rank",
        "Reference Rank",
        UMI_DEVELOPER_INTELLIGENCE_RETRIEVAL,
        60.0,
        50.0,
        0.20,
        0.35,
        0.30,
        0.15,
        1U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
