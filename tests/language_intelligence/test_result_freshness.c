/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_result_freshness.c
 * PURPOSE: Focused regression test for result freshness.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/result_freshness.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceResultFreshness entry;
    umi_language_intelligence_result_freshness_init(&entry, "key", 42U, 100U, 20U);
    CHECK(umi_language_intelligence_result_freshness_is_fresh(&entry, 110U, 42U) != 0);
    CHECK(umi_language_intelligence_result_freshness_is_fresh(&entry, 121U, 42U) == 0);
    umi_language_intelligence_result_freshness_invalidate(&entry);
    CHECK(umi_language_intelligence_result_freshness_is_fresh(&entry, 110U, 42U) == 0);
    return 0;
}
