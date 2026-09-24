/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/compilation_database.c
 *
 * PURPOSE:
 *   Apply the shared Compilation Database policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/compilation_database.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageCompilationDatabase(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.compilation-database",
        "Compilation Database",
        UMI_DEVELOPER_INTELLIGENCE_FRESHNESS,
        60.0,
        40.0,
        0.10,
        0.30,
        0.35,
        0.25,
        1U,
        0,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
