/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/response_freshness.c
 *
 * PURPOSE:
 *   Apply the shared Language Response Freshness policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/response_freshness.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageResponseFreshness(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.response-freshness",
        "Language Response Freshness",
        UMI_DEVELOPER_INTELLIGENCE_FRESHNESS,
        65.0,
        45.0,
        0.05,
        0.25,
        0.50,
        0.20,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
