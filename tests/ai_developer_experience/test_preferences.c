/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_preferences.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience preferences.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/preferences.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiDeveloperPreferences preferences;

    umi_ai_developer_preferences_init(&preferences);
    assert(preferences.diff_layout ==
           UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE);
    assert(preferences.visible_rows == 24U);
    assert(umi_ai_developer_preferences_validate(
        &preferences) == UMI_STATUS_OK);

    preferences.visible_rows = 0U;
    assert(umi_ai_developer_preferences_validate(
        &preferences) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

