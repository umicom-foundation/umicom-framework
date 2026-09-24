/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/completion_prefix.c
 *
 * PURPOSE:
 *   Apply the shared Completion Prefix policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/completion_prefix.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCompletionPrefix(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.completion-prefix",
        "Completion Prefix",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        55.0,
        65.0,
        0.40,
        0.15,
        0.30,
        0.15,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
