/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_type_definition_query.c
 * PURPOSE: Focused regression test for type definition query.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/type_definition_query.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceTypeDefinitionQuery query;
    umi_language_intelligence_type_definition_query_init(&query, "Hash");
    CHECK(umi_language_intelligence_type_definition_query_matches(&query, "hash_table") != 0);
    CHECK(umi_language_intelligence_type_definition_query_candidate_score(&query, "Hash") >
           umi_language_intelligence_type_definition_query_candidate_score(&query, "MyHashValue"));
    CHECK(umi_language_intelligence_type_definition_query_matches(&query, "vector") == 0);
    return 0;
}
