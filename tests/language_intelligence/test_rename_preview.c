/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_rename_preview.c
 * PURPOSE: Focused regression test for rename preview.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/rename_preview.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceRenamePreviewInput input = {0};
    input.required_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION;
    input.available_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_ALL;
    input.threshold = 10U;
    input.risk = 2U;
    CHECK(umi_language_intelligence_rename_preview_capabilities_satisfied(&input) != 0);
    CHECK(umi_language_intelligence_rename_preview_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_ALLOW);
    input.stale_age = 20U;
    CHECK(umi_language_intelligence_rename_preview_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_ALLOW_WITH_REFRESH);
    input.hard_block = 1;
    CHECK(umi_language_intelligence_rename_preview_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_DENY);
    return 0;
}
