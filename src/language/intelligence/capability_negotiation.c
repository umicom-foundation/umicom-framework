/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/capability_negotiation.c
 *
 * PURPOSE:
 *   Implement negotiate required and optional language capabilities across provider boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/capability_negotiation.h"
/*
 * Provide the language intelligence capability negotiation capabilities satisfied
 * operation used by this module and its client applications.
 */
int umi_language_intelligence_capability_negotiation_capabilities_satisfied(
    const UmiLanguageIntelligenceCapabilityNegotiationInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
/*
 * Provide the language intelligence capability negotiation evaluate operation used by this
 * module and its client applications.
 */
UmiLanguageIntelligenceCapabilityNegotiationDecision umi_language_intelligence_capability_negotiation_evaluate(
    const UmiLanguageIntelligenceCapabilityNegotiationInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_language_intelligence_capability_negotiation_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_ALLOW;
}
