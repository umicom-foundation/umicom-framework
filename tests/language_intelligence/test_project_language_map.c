/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_project_language_map.c
 * PURPOSE: Focused regression test for project language map.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/project_language_map.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceProjectLanguageMap mapping;
    umi_language_intelligence_project_language_map_init(&mapping);
    CHECK(umi_language_intelligence_project_language_map_set(&mapping, "c", "clangd", "workspace") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_project_language_map_validate(&mapping) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_project_language_map_matches(&mapping, "c", "workspace") != 0);
    CHECK(umi_language_intelligence_project_language_map_matches(&mapping, "cpp", "workspace") == 0);
    return 0;
}
