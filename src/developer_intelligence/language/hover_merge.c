/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/hover_merge.c
 *
 * PURPOSE:
 *   Apply the shared Hover Merge policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/hover_merge.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageHoverMerge(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.hover-merge",
        "Hover Merge",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        55.0,
        60.0,
        0.10,
        0.40,
        0.30,
        0.20,
        1U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageHoverMergeBatch, UmiDeveloperIntelligenceEvaluateLanguageHoverMerge,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
