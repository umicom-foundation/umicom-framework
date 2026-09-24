/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/request_budget.c
 *
 * PURPOSE:
 *   Apply the shared Language Request Budget policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/request_budget.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageRequestBudget(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.request-budget",
        "Language Request Budget",
        UMI_DEVELOPER_INTELLIGENCE_BUDGET,
        0.0,
        50.0,
        0.10,
        0.20,
        0.20,
        0.50,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
