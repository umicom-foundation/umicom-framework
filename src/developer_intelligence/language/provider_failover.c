/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/provider_failover.c
 *
 * PURPOSE:
 *   Apply the shared Language Provider Failover policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/provider_failover.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageProviderFailover(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.provider-failover",
        "Language Provider Failover",
        UMI_DEVELOPER_INTELLIGENCE_ROUTE,
        50.0,
        50.0,
        0.05,
        0.20,
        0.25,
        0.50,
        0U,
        0,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
