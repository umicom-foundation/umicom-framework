/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_implementation_query.c
 * PURPOSE: Focused regression test for implementation query.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/implementation_query.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceImplementationQuery query;
    umi_language_intelligence_implementation_query_init(&query, "Hash");
    CHECK(umi_language_intelligence_implementation_query_matches(&query, "hash_table") != 0);
    CHECK(umi_language_intelligence_implementation_query_candidate_score(&query, "Hash") >
           umi_language_intelligence_implementation_query_candidate_score(&query, "MyHashValue"));
    CHECK(umi_language_intelligence_implementation_query_matches(&query, "vector") == 0);
    return 0;
}
