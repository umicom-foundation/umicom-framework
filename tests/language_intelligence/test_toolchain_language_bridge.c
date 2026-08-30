/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_toolchain_language_bridge.c
 * PURPOSE: Focused regression test for toolchain language bridge.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/toolchain_language_bridge.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceToolchainLanguageBridge mapping;
    umi_language_intelligence_toolchain_language_bridge_init(&mapping);
    CHECK(umi_language_intelligence_toolchain_language_bridge_set(&mapping, "c", "clangd", "workspace") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_toolchain_language_bridge_validate(&mapping) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_toolchain_language_bridge_matches(&mapping, "c", "workspace") != 0);
    CHECK(umi_language_intelligence_toolchain_language_bridge_matches(&mapping, "cpp", "workspace") == 0);
    return 0;
}
