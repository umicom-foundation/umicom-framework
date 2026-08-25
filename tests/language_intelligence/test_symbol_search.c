/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_symbol_search.c
 * PURPOSE: Focused regression test for symbol search.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/symbol_search.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceSymbolSearch query;
    umi_language_intelligence_symbol_search_init(&query, "Hash");
    CHECK(umi_language_intelligence_symbol_search_matches(&query, "hash_table") != 0);
    CHECK(umi_language_intelligence_symbol_search_candidate_score(&query, "Hash") >
           umi_language_intelligence_symbol_search_candidate_score(&query, "MyHashValue"));
    CHECK(umi_language_intelligence_symbol_search_matches(&query, "vector") == 0);
    return 0;
}
