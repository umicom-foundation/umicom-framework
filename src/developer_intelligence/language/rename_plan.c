/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/rename_plan.c
 *
 * PURPOSE:
 *   Apply the shared Rename Plan policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/rename_plan.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageRenamePlan(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.rename-plan",
        "Rename Plan",
        UMI_DEVELOPER_INTELLIGENCE_APPROVAL,
        65.0,
        30.0,
        0.15,
        0.30,
        0.35,
        0.20,
        1U,
        1,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
