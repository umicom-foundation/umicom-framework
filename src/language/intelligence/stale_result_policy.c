/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/stale_result_policy.c
 *
 * PURPOSE:
 *   Implement decide whether stale provider results may be displayed, refreshed or rejected.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/stale_result_policy.h"
int umi_language_intelligence_stale_result_policy_capabilities_satisfied(
    const UmiLanguageIntelligenceStaleResultPolicyInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceStaleResultPolicyDecision umi_language_intelligence_stale_result_policy_evaluate(
    const UmiLanguageIntelligenceStaleResultPolicyInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_DENY;
    if (!umi_language_intelligence_stale_result_policy_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW;
}
