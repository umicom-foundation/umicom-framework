/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_preferences.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience preferences.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/preferences.h"

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

