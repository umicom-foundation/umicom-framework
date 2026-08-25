/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_semantic_refresh.c
 * PURPOSE: Focused regression test for semantic refresh.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/semantic_refresh.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceSemanticRefresh value;
    umi_language_intelligence_semantic_refresh_init(&value, "semantic_refresh");
    CHECK(umi_language_intelligence_semantic_refresh_transition(&value, UMI_LANGUAGE_INTELLIGENCE_STARTING, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_semantic_refresh_transition(&value, UMI_LANGUAGE_INTELLIGENCE_READY, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_semantic_refresh_is_usable(&value) != 0);
    umi_language_intelligence_semantic_refresh_record_failure(&value);
    CHECK(value.state == UMI_LANGUAGE_INTELLIGENCE_DEGRADED);
    return 0;
}
