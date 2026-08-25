/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/capability_negotiation.c
 *
 * PURPOSE:
 *   Implement negotiate required and optional language capabilities across provider boundaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/capability_negotiation.h"
int umi_language_intelligence_capability_negotiation_capabilities_satisfied(
    const UmiLanguageIntelligenceCapabilityNegotiationInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceCapabilityNegotiationDecision umi_language_intelligence_capability_negotiation_evaluate(
    const UmiLanguageIntelligenceCapabilityNegotiationInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_DENY;
    if (!umi_language_intelligence_capability_negotiation_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_ALLOW;
}
