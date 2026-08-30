/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_diagnostics_pull_state.c
 * PURPOSE: Focused regression test for diagnostics pull state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_pull_state.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceDiagnosticsPullState value;
    umi_language_intelligence_diagnostics_pull_state_init(&value, "diagnostics_pull_state");
    CHECK(umi_language_intelligence_diagnostics_pull_state_transition(&value, UMI_LANGUAGE_INTELLIGENCE_STARTING, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_diagnostics_pull_state_transition(&value, UMI_LANGUAGE_INTELLIGENCE_READY, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_diagnostics_pull_state_is_usable(&value) != 0);
    umi_language_intelligence_diagnostics_pull_state_record_failure(&value);
    CHECK(value.state == UMI_LANGUAGE_INTELLIGENCE_DEGRADED);
    return 0;
}
