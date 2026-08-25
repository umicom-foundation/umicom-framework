/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_workspace_configuration.c
 * PURPOSE: Focused regression test for workspace configuration.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/workspace_configuration.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceWorkspaceConfiguration value;
    UmiLanguageIntelligenceWorkspaceConfiguration other;
    umi_language_intelligence_workspace_configuration_init(&value, "workspace_configuration.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_workspace_configuration_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_configuration_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_configuration_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_workspace_configuration_init(&other, "workspace_configuration.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_workspace_configuration_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
