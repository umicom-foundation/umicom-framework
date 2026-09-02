/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/semantic_delta.c
 *
 * PURPOSE:
 *   Implement validate whether semantic-token delta application is safe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/semantic_delta.h"
/*
 * Provide the language intelligence semantic delta capabilities satisfied operation used
 * by this module and its client applications.
 */
int umi_language_intelligence_semantic_delta_capabilities_satisfied(
    const UmiLanguageIntelligenceSemanticDeltaInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
/*
 * Provide the language intelligence semantic delta evaluate operation used by this module
 * and its client applications.
 */
UmiLanguageIntelligenceSemanticDeltaDecision umi_language_intelligence_semantic_delta_evaluate(
    const UmiLanguageIntelligenceSemanticDeltaInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_language_intelligence_semantic_delta_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_ALLOW;
}
