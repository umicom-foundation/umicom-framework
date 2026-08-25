/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_response_cache.c
 * PURPOSE: Focused regression test for response cache.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/response_cache.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceResponseCache entry;
    umi_language_intelligence_response_cache_init(&entry, "key", 42U, 100U, 20U);
    CHECK(umi_language_intelligence_response_cache_is_fresh(&entry, 110U, 42U) != 0);
    CHECK(umi_language_intelligence_response_cache_is_fresh(&entry, 121U, 42U) == 0);
    umi_language_intelligence_response_cache_invalidate(&entry);
    CHECK(umi_language_intelligence_response_cache_is_fresh(&entry, 110U, 42U) == 0);
    return 0;
}
