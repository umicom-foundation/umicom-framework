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

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "umicom/ai_developer_experience/preferences.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ai_developer_experience/preferences.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAiDeveloperPreferencesTransferEqual(const UmiAiDeveloperPreferences *a, const UmiAiDeveloperPreferences *b)
{
    return a->diff_layout == b->diff_layout &&
        a->diff_context_lines == b->diff_context_lines &&
        a->visible_rows == b->visible_rows &&
        a->auto_follow_active_task == b->auto_follow_active_task &&
        a->auto_open_review == b->auto_open_review &&
        a->show_tool_arguments == b->show_tool_arguments &&
        a->show_validation_output == b->show_validation_output &&
        a->show_context_token_estimates == b->show_context_token_estimates &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAiDeveloperPreferencesTransferTails(UmiAiDeveloperPreferences *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAiDeveloperPreferencesTransferMalformed(const UmiAiDeveloperPreferences *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAiDeveloperPreferencesTransferCases, UmiAiDeveloperPreferences,
    umi_ai_developer_preferences_archive_encode, umi_ai_developer_preferences_archive_decode,
    UmiAiDeveloperPreferencesTransferEqual, UmiAiDeveloperPreferencesTransferTails, UmiAiDeveloperPreferencesTransferMalformed)

int main(void)
{
    UmiAiDeveloperPreferences preferences;

    umi_ai_developer_preferences_init(&preferences);
    assert(preferences.diff_layout ==
           UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE);
    assert(preferences.visible_rows == 24U);
    assert(umi_ai_developer_preferences_validate(
        &preferences) == UMI_STATUS_OK);
    if (UmiAiDeveloperPreferencesTransferCases(&preferences) != 0) return 1;


    preferences.visible_rows = 0U;
    assert(umi_ai_developer_preferences_validate(
        &preferences) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

