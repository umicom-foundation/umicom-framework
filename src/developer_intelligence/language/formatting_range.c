/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/formatting_range.c
 *
 * PURPOSE:
 *   Apply the shared Formatting Range policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/formatting_range.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageFormattingRange(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.formatting-range",
        "Formatting Range",
        UMI_DEVELOPER_INTELLIGENCE_TRUST,
        50.0,
        35.0,
        0.10,
        0.25,
        0.40,
        0.25,
        0U,
        1,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
