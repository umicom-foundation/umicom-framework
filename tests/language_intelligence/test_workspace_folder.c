/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_workspace_folder.c
 * PURPOSE: Focused regression test for workspace folder.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/workspace_folder.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceWorkspaceFolder value;
    UmiLanguageIntelligenceWorkspaceFolder other;
    umi_language_intelligence_workspace_folder_init(&value, "workspace_folder.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_workspace_folder_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_folder_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_folder_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_workspace_folder_init(&other, "workspace_folder.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_workspace_folder_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
