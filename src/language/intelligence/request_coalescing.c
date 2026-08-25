/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/request_coalescing.c
 *
 * PURPOSE:
 *   Implement detect equivalent requests that may be safely coalesced.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_coalescing.h"
int umi_language_intelligence_request_coalescing_capabilities_satisfied(
    const UmiLanguageIntelligenceRequestCoalescingInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceRequestCoalescingDecision umi_language_intelligence_request_coalescing_evaluate(
    const UmiLanguageIntelligenceRequestCoalescingInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY;
    if (!umi_language_intelligence_request_coalescing_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW;
}
