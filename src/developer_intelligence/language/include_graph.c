/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/include_graph.c
 *
 * PURPOSE:
 *   Apply the shared Include Graph policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/include_graph.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageIncludeGraph(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.include-graph",
        "Include Graph",
        UMI_DEVELOPER_INTELLIGENCE_CONFLICT,
        0.0,
        40.0,
        0.15,
        0.35,
        0.30,
        0.20,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
