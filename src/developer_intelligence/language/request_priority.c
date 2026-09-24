/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/request_priority.c
 *
 * PURPOSE:
 *   Apply the shared Language Request Priority policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/request_priority.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageRequestPriority(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.request-priority",
        "Language Request Priority",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        55.0,
        60.0,
        0.10,
        0.15,
        0.50,
        0.25,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
