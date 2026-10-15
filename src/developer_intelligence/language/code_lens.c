/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/code_lens.c
 *
 * PURPOSE:
 *   Apply the shared Code Lens policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/code_lens.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCodeLens(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.code-lens",
        "Code Lens",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        55.0,
        55.0,
        0.15,
        0.30,
        0.35,
        0.20,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageCodeLensBatch, UmiDeveloperIntelligenceEvaluateLanguageCodeLens,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
