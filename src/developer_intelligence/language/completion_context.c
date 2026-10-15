/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/completion_context.c
 *
 * PURPOSE:
 *   Apply the shared Completion Context policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/completion_context.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCompletionContext(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.completion-context",
        "Completion Context",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        60.0,
        60.0,
        0.20,
        0.20,
        0.45,
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageCompletionContextBatch, UmiDeveloperIntelligenceEvaluateLanguageCompletionContext,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
