/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_workspace_edit_validation.c
 * PURPOSE: Focused regression test for workspace edit validation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/workspace_edit_validation.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceWorkspaceEditValidationInput input = {0};
    input.required_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION;
    input.available_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_ALL;
    input.threshold = 10U;
    input.risk = 2U;
    CHECK(umi_language_intelligence_workspace_edit_validation_capabilities_satisfied(&input) != 0);
    CHECK(umi_language_intelligence_workspace_edit_validation_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW);
    input.stale_age = 20U;
    CHECK(umi_language_intelligence_workspace_edit_validation_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW_WITH_REFRESH);
    input.hard_block = 1;
    CHECK(umi_language_intelligence_workspace_edit_validation_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY);
    return 0;
}
