/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/refactor_preview.c
 *
 * PURPOSE:
 *   Apply the shared Refactor Preview policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/refactor_preview.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageRefactorPreview(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.refactor-preview",
        "Refactor Preview",
        UMI_DEVELOPER_INTELLIGENCE_APPROVAL,
        65.0,
        30.0,
        0.10,
        0.35,
        0.35,
        0.20,
        1U,
        1,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
