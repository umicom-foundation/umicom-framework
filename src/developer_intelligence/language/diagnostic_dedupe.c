/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/diagnostic_dedupe.c
 *
 * PURPOSE:
 *   Apply the shared Diagnostic Deduplication policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/diagnostic_dedupe.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageDiagnosticDedupe(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.diagnostic-dedupe",
        "Diagnostic Deduplication",
        UMI_DEVELOPER_INTELLIGENCE_CONFLICT,
        0.0,
        50.0,
        0.20,
        0.30,
        0.30,
        0.20,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
