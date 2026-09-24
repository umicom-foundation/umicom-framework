/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/rename_conflict.c
 *
 * PURPOSE:
 *   Apply the shared Rename Conflict policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/rename_conflict.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageRenameConflict(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.rename-conflict",
        "Rename Conflict",
        UMI_DEVELOPER_INTELLIGENCE_CONFLICT,
        0.0,
        30.0,
        0.20,
        0.30,
        0.30,
        0.20,
        0U,
        1,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
