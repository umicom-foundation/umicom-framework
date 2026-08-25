/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_provider_affinity.c
 * PURPOSE: Focused regression test for provider affinity.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_affinity.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceProviderAffinity mapping;
    umi_language_intelligence_provider_affinity_init(&mapping);
    CHECK(umi_language_intelligence_provider_affinity_set(&mapping, "c", "clangd", "workspace") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_affinity_validate(&mapping) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_affinity_matches(&mapping, "c", "workspace") != 0);
    CHECK(umi_language_intelligence_provider_affinity_matches(&mapping, "cpp", "workspace") == 0);
    return 0;
}
