/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/selection_range.c
 *
 * PURPOSE:
 *   Apply the shared Selection Range policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/selection_range.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageSelectionRange(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.selection-range",
        "Selection Range",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        55.0,
        50.0,
        0.20,
        0.25,
        0.40,
        0.15,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageSelectionRangeBatch, UmiDeveloperIntelligenceEvaluateLanguageSelectionRange,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
