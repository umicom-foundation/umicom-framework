/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_types.c
 * PURPOSE: Focused regression test for shared language-intelligence types.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/types.h"

#include <stdint.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceRange outer;
    UmiLanguageIntelligenceRange inner;
    char text[16];
    uint64_t hash;

    umi_language_intelligence_types_init_range(&outer, 1U, 0U, 5U, 20U);
    umi_language_intelligence_types_init_range(&inner, 2U, 1U, 3U, 5U);
    CHECK(umi_language_intelligence_range_is_valid(&outer) != 0);
    CHECK(umi_language_intelligence_range_contains(&outer, &inner) != 0);
    CHECK(umi_language_intelligence_copy_text(text, sizeof(text), "clangd") ==
           UMI_STATUS_OK);
    CHECK(strcmp(text, "clangd") == 0);
    hash = umi_language_intelligence_hash_text("clangd");
    CHECK(hash != 0U);
    CHECK(umi_language_intelligence_score_saturating_add(UINT64_MAX, 1U) ==
           UINT64_MAX);
    CHECK(strcmp(umi_language_intelligence_state_text(
        UMI_LANGUAGE_INTELLIGENCE_READY), "ready") == 0);
    return 0;
}
