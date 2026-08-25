/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_declaration_query.c
 * PURPOSE: Focused regression test for declaration query.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/declaration_query.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceDeclarationQuery query;
    umi_language_intelligence_declaration_query_init(&query, "Hash");
    CHECK(umi_language_intelligence_declaration_query_matches(&query, "hash_table") != 0);
    CHECK(umi_language_intelligence_declaration_query_candidate_score(&query, "Hash") >
           umi_language_intelligence_declaration_query_candidate_score(&query, "MyHashValue"));
    CHECK(umi_language_intelligence_declaration_query_matches(&query, "vector") == 0);
    return 0;
}
