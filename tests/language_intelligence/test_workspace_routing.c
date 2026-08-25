/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_workspace_routing.c
 * PURPOSE: Focused regression test for workspace routing.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/workspace_routing.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceWorkspaceRouting mapping;
    umi_language_intelligence_workspace_routing_init(&mapping);
    CHECK(umi_language_intelligence_workspace_routing_set(&mapping, "c", "clangd", "workspace") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_routing_validate(&mapping) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_routing_matches(&mapping, "c", "workspace") != 0);
    CHECK(umi_language_intelligence_workspace_routing_matches(&mapping, "cpp", "workspace") == 0);
    return 0;
}
