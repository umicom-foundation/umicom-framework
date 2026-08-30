/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_provider_priority.c
 * PURPOSE: Focused regression test for provider priority.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_priority.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceProviderPriorityInput left;
    UmiLanguageIntelligenceProviderPriorityInput right;
    umi_language_intelligence_provider_priority_input_init(&left);
    umi_language_intelligence_provider_priority_input_init(&right);
    left.base_priority = 10U;
    left.exact_match = 1U;
    left.health = 10U;
    right.base_priority = 20U;
    CHECK(umi_language_intelligence_provider_priority_score(&left) > umi_language_intelligence_provider_priority_score(&right));
    CHECK(umi_language_intelligence_provider_priority_prefer(&left, &right) != 0);
    return 0;
}
