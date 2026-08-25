/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/semantic_delta.c
 *
 * PURPOSE:
 *   Implement validate whether semantic-token delta application is safe.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/semantic_delta.h"
int umi_language_intelligence_semantic_delta_capabilities_satisfied(
    const UmiLanguageIntelligenceSemanticDeltaInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceSemanticDeltaDecision umi_language_intelligence_semantic_delta_evaluate(
    const UmiLanguageIntelligenceSemanticDeltaInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_DENY;
    if (!umi_language_intelligence_semantic_delta_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_ALLOW;
}
