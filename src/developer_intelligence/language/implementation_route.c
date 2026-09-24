/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/implementation_route.c
 *
 * PURPOSE:
 *   Apply the shared Implementation Route policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/implementation_route.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageImplementationRoute(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.implementation-route",
        "Implementation Route",
        UMI_DEVELOPER_INTELLIGENCE_ROUTE,
        55.0,
        45.0,
        0.10,
        0.30,
        0.30,
        0.30,
        0U,
        0,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
