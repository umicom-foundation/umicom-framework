/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/type_definition_route.c
 *
 * PURPOSE:
 *   Apply the shared Type Definition Route policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/developer_intelligence/language/type_definition_route.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageTypeDefinitionRoute(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.type-definition-route",
        "Type Definition Route",
        UMI_DEVELOPER_INTELLIGENCE_ROUTE,
        55.0,
        45.0,
        0.10,
        0.35,
        0.30,
        0.25,
        0U,
        0,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageTypeDefinitionRouteBatch, UmiDeveloperIntelligenceEvaluateLanguageTypeDefinitionRoute,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
