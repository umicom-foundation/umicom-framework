/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_server_trace.c
 * PURPOSE: Focused regression test for server trace.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/server_trace.h"
#include <string.h>
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceServerTrace catalogue;
    UmiLanguageIntelligenceServerTraceItem first = {0};
    UmiLanguageIntelligenceServerTraceItem second = {0};
    const UmiLanguageIntelligenceServerTraceItem *best;
    umi_language_intelligence_server_trace_init(&catalogue);
    (void)umi_language_intelligence_copy_text(first.id, sizeof(first.id), "one");
    (void)umi_language_intelligence_copy_text(first.label, sizeof(first.label), "One");
    first.enabled = 1; first.priority = 10U; first.fingerprint = 1U;
    (void)umi_language_intelligence_copy_text(second.id, sizeof(second.id), "two");
    second.enabled = 1; second.priority = 20U; second.fingerprint = 2U;
    CHECK(umi_language_intelligence_server_trace_upsert(&catalogue, &first) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_server_trace_upsert(&catalogue, &second) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_server_trace_find(&catalogue, "one") != NULL);
    best = umi_language_intelligence_server_trace_best(&catalogue);
    CHECK(best != NULL && strcmp(best->id, "two") == 0);
    return 0;
}
