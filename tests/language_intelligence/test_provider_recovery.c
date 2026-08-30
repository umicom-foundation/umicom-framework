/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_provider_recovery.c
 * PURPOSE: Focused regression test for provider recovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_recovery.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceProviderRecoveryInput input = {0};
    input.required_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION;
    input.available_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_ALL;
    input.threshold = 10U;
    input.risk = 2U;
    CHECK(umi_language_intelligence_provider_recovery_capabilities_satisfied(&input) != 0);
    CHECK(umi_language_intelligence_provider_recovery_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW);
    input.stale_age = 20U;
    CHECK(umi_language_intelligence_provider_recovery_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW_WITH_REFRESH);
    input.hard_block = 1;
    CHECK(umi_language_intelligence_provider_recovery_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY);
    return 0;
}
