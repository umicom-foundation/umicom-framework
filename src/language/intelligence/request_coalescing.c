/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/request_coalescing.c
 *
 * PURPOSE:
 *   Implement detect equivalent requests that may be safely coalesced.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_coalescing.h"
/*
 * Provide the language intelligence request coalescing capabilities satisfied operation
 * used by this module and its client applications.
 */
int umi_language_intelligence_request_coalescing_capabilities_satisfied(
    const UmiLanguageIntelligenceRequestCoalescingInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
/*
 * Provide the language intelligence request coalescing evaluate operation used by this
 * module and its client applications.
 */
UmiLanguageIntelligenceRequestCoalescingDecision umi_language_intelligence_request_coalescing_evaluate(
    const UmiLanguageIntelligenceRequestCoalescingInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_language_intelligence_request_coalescing_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW;
}
