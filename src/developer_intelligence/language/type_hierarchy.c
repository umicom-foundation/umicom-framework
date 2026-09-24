/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/type_hierarchy.c
 *
 * PURPOSE:
 *   Apply the shared Type Hierarchy policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/type_hierarchy.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageTypeHierarchy(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.type-hierarchy",
        "Type Hierarchy",
        UMI_DEVELOPER_INTELLIGENCE_RETRIEVAL,
        60.0,
        50.0,
        0.15,
        0.40,
        0.30,
        0.15,
        1U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
