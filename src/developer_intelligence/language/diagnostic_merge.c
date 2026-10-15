/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/diagnostic_merge.c
 *
 * PURPOSE:
 *   Apply the shared Diagnostic Merge policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/diagnostic_merge.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageDiagnosticMerge(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.diagnostic-merge",
        "Diagnostic Merge",
        UMI_DEVELOPER_INTELLIGENCE_CONFLICT,
        0.0,
        50.0,
        0.10,
        0.35,
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageDiagnosticMergeBatch, UmiDeveloperIntelligenceEvaluateLanguageDiagnosticMerge,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
