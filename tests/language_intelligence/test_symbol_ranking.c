/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_symbol_ranking.c
 * PURPOSE: Focused regression test for symbol ranking.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/symbol_ranking.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceSymbolRankingInput left;
    UmiLanguageIntelligenceSymbolRankingInput right;
    umi_language_intelligence_symbol_ranking_input_init(&left);
    umi_language_intelligence_symbol_ranking_input_init(&right);
    left.base_priority = 10U;
    left.exact_match = 1U;
    left.health = 10U;
    right.base_priority = 20U;
    CHECK(umi_language_intelligence_symbol_ranking_score(&left) > umi_language_intelligence_symbol_ranking_score(&right));
    CHECK(umi_language_intelligence_symbol_ranking_prefer(&left, &right) != 0);
    return 0;
}
