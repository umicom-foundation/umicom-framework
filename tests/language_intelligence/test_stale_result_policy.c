/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_stale_result_policy.c
 * PURPOSE: Focused regression test for stale result policy.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/stale_result_policy.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceStaleResultPolicyInput input = {0};
    input.required_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION;
    input.available_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_ALL;
    input.threshold = 10U;
    input.risk = 2U;
    CHECK(umi_language_intelligence_stale_result_policy_capabilities_satisfied(&input) != 0);
    CHECK(umi_language_intelligence_stale_result_policy_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW);
    input.stale_age = 20U;
    CHECK(umi_language_intelligence_stale_result_policy_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW_WITH_REFRESH);
    input.hard_block = 1;
    CHECK(umi_language_intelligence_stale_result_policy_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_DENY);
    return 0;
}
