/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_diagnostics_deduplication.c
 * PURPOSE: Focused regression test for diagnostics deduplication.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_deduplication.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceDiagnosticsDeduplicationInput input = {0};
    input.required_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION;
    input.available_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_ALL;
    input.threshold = 10U;
    input.risk = 2U;
    CHECK(umi_language_intelligence_diagnostics_deduplication_capabilities_satisfied(&input) != 0);
    CHECK(umi_language_intelligence_diagnostics_deduplication_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW);
    input.stale_age = 20U;
    CHECK(umi_language_intelligence_diagnostics_deduplication_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW_WITH_REFRESH);
    input.hard_block = 1;
    CHECK(umi_language_intelligence_diagnostics_deduplication_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY);
    return 0;
}
