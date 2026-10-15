/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/symbol_search.c
 *
 * PURPOSE:
 *   Apply the shared Workspace Symbol Search policy without replacing the canonical
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
#include "umicom/developer_intelligence/language/symbol_search.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageSymbolSearch(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.symbol-search",
        "Workspace Symbol Search",
        UMI_DEVELOPER_INTELLIGENCE_RETRIEVAL,
        55.0,
        55.0,
        0.40,
        0.25,
        0.20,
        0.15,
        1U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateLanguageSymbolSearchBatch, UmiDeveloperIntelligenceEvaluateLanguageSymbolSearch,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
