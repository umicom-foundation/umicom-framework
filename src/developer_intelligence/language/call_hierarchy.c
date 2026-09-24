/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/call_hierarchy.c
 *
 * PURPOSE:
 *   Apply the shared Call Hierarchy policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/call_hierarchy.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCallHierarchy(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.call-hierarchy",
        "Call Hierarchy",
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
