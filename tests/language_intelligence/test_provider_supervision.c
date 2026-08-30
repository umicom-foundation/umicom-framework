/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_provider_supervision.c
 * PURPOSE: Focused regression test for provider supervision.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_supervision.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceProviderSupervision value;
    umi_language_intelligence_provider_supervision_init(&value, "provider_supervision");
    CHECK(umi_language_intelligence_provider_supervision_transition(&value, UMI_LANGUAGE_INTELLIGENCE_STARTING, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_supervision_transition(&value, UMI_LANGUAGE_INTELLIGENCE_READY, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_supervision_is_usable(&value) != 0);
    umi_language_intelligence_provider_supervision_record_failure(&value);
    CHECK(value.state == UMI_LANGUAGE_INTELLIGENCE_DEGRADED);
    return 0;
}
