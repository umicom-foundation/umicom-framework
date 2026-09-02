/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_provider_quarantine.c
 * PURPOSE: Focused regression test for provider quarantine.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_quarantine.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceProviderQuarantine value;
    umi_language_intelligence_provider_quarantine_init(&value, "provider_quarantine");
    CHECK(umi_language_intelligence_provider_quarantine_transition(&value, UMI_LANGUAGE_INTELLIGENCE_STARTING, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_quarantine_transition(&value, UMI_LANGUAGE_INTELLIGENCE_READY, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_quarantine_is_usable(&value) != 0);
    umi_language_intelligence_provider_quarantine_record_failure(&value);
    CHECK(value.state == UMI_LANGUAGE_INTELLIGENCE_DEGRADED);
    return 0;
}
