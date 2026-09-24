/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/assembly_profile.c
 *
 * PURPOSE:
 *   Apply the shared Assembly Language Profile policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/assembly_profile.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageAssemblyProfile(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.assembly-profile",
        "Assembly Language Profile",
        UMI_DEVELOPER_INTELLIGENCE_ROUTE,
        60.0,
        35.0,
        0.10,
        0.25,
        0.30,
        0.35,
        0U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
