/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/cpp_profile.c
 *
 * PURPOSE:
 *   Apply the shared C++ Language Profile policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/cpp_profile.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCppProfile(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.cpp-profile",
        "C++ Language Profile",
        UMI_DEVELOPER_INTELLIGENCE_ROUTE,
        60.0,
        35.0,
        0.10,
        0.25,
        0.30,
        0.35,
        0U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageCppProfileBatch, UmiDeveloperIntelligenceEvaluateLanguageCppProfile,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
