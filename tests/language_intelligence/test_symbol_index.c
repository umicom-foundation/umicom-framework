/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_symbol_index.c
 * PURPOSE: Focused regression test for symbol index.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/symbol_index.h"
#include <string.h>
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceSymbolIndex catalogue;
    UmiLanguageIntelligenceSymbolIndexItem first = {0};
    UmiLanguageIntelligenceSymbolIndexItem second = {0};
    const UmiLanguageIntelligenceSymbolIndexItem *best;
    umi_language_intelligence_symbol_index_init(&catalogue);
    (void)umi_language_intelligence_copy_text(first.id, sizeof(first.id), "one");
    (void)umi_language_intelligence_copy_text(first.label, sizeof(first.label), "One");
    first.enabled = 1; first.priority = 10U; first.fingerprint = 1U;
    (void)umi_language_intelligence_copy_text(second.id, sizeof(second.id), "two");
    second.enabled = 1; second.priority = 20U; second.fingerprint = 2U;
    CHECK(umi_language_intelligence_symbol_index_upsert(&catalogue, &first) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_symbol_index_upsert(&catalogue, &second) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_symbol_index_find(&catalogue, "one") != NULL);
    best = umi_language_intelligence_symbol_index_best(&catalogue);
    CHECK(best != NULL && strcmp(best->id, "two") == 0);
    return 0;
}
