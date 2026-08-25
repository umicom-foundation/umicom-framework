/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_diagnostics_baseline.c
 * PURPOSE: Focused regression test for diagnostics baseline.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_baseline.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceDiagnosticsBaseline entry;
    umi_language_intelligence_diagnostics_baseline_init(&entry, "key", 42U, 100U, 20U);
    CHECK(umi_language_intelligence_diagnostics_baseline_is_fresh(&entry, 110U, 42U) != 0);
    CHECK(umi_language_intelligence_diagnostics_baseline_is_fresh(&entry, 121U, 42U) == 0);
    umi_language_intelligence_diagnostics_baseline_invalidate(&entry);
    CHECK(umi_language_intelligence_diagnostics_baseline_is_fresh(&entry, 110U, 42U) == 0);
    return 0;
}
