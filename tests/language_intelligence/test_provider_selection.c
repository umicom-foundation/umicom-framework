/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_provider_selection.c
 * PURPOSE: Focused regression test for provider selection.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_selection.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceProviderSelectionInput left;
    UmiLanguageIntelligenceProviderSelectionInput right;
    umi_language_intelligence_provider_selection_input_init(&left);
    umi_language_intelligence_provider_selection_input_init(&right);
    left.base_priority = 10U;
    left.exact_match = 1U;
    left.health = 10U;
    right.base_priority = 20U;
    CHECK(umi_language_intelligence_provider_selection_score(&left) > umi_language_intelligence_provider_selection_score(&right));
    CHECK(umi_language_intelligence_provider_selection_prefer(&left, &right) != 0);
    return 0;
}
