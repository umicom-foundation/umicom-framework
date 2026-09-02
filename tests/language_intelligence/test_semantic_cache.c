/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_semantic_cache.c
 * PURPOSE: Focused regression test for semantic cache.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/semantic_cache.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceSemanticCache entry;
    umi_language_intelligence_semantic_cache_init(&entry, "key", 42U, 100U, 20U);
    CHECK(umi_language_intelligence_semantic_cache_is_fresh(&entry, 110U, 42U) != 0);
    CHECK(umi_language_intelligence_semantic_cache_is_fresh(&entry, 121U, 42U) == 0);
    umi_language_intelligence_semantic_cache_invalidate(&entry);
    CHECK(umi_language_intelligence_semantic_cache_is_fresh(&entry, 110U, 42U) == 0);
    return 0;
}
