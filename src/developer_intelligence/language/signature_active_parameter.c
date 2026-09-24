/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/signature_active_parameter.c
 *
 * PURPOSE:
 *   Apply the shared Signature Active Parameter policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/signature_active_parameter.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageSignatureActiveParameter(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.signature-active-parameter",
        "Signature Active Parameter",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        60.0,
        55.0,
        0.25,
        0.25,
        0.35,
        0.15,
        0U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
