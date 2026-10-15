/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/local_runtime_route.c
 *
 * PURPOSE:
 *   Apply the shared Local AI Runtime Route policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/local_runtime_route.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiLocalRuntimeRoute(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.local-runtime-route",
        "Local AI Runtime Route",
        UMI_DEVELOPER_INTELLIGENCE_ROUTE,
        55.0,
        35.0,
        0.05,
        0.25,
        0.25,
        0.45,
        0U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiLocalRuntimeRouteBatch, UmiDeveloperIntelligenceEvaluateAiLocalRuntimeRoute,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
