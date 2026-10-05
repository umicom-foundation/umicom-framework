/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/preferences.c
 *
 * PURPOSE:
 *   Implement conservative defaults and validation for AI developer UI behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/preferences.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise ai developer preferences from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_developer_preferences_init(
    UmiAiDeveloperPreferences *preferences)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preferences == NULL) return;

    (void)memset(preferences, 0, sizeof(*preferences));
    preferences->diff_layout = UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE;
    preferences->diff_context_lines = 3U;
    preferences->visible_rows = 24U;
    preferences->auto_follow_active_task = 1;
    preferences->auto_open_review = 1;
    preferences->show_tool_arguments = 0;
    preferences->show_validation_output = 1;
    preferences->show_context_token_estimates = 1;
    preferences->revision = 1U;
}

/*
 * Check that ai developer preferences satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ai_developer_preferences_validate(
    const UmiAiDeveloperPreferences *preferences)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preferences == NULL ||
        preferences->diff_layout < UMI_AI_DEVELOPER_DIFF_LAYOUT_UNIFIED ||
        preferences->diff_layout > UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE ||
        preferences->diff_context_lines > 20U ||
        preferences->visible_rows == 0U ||
        preferences->visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAiDeveloperPreferencesArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2dfd85d179b1f17b);

    return schema;
}
static size_t UmiAiDeveloperPreferencesArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAiDeveloperPreferencesArchiveWrite(UmiArchiveWriter *writer, const UmiAiDeveloperPreferences *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->diff_layout);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->diff_context_lines);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible_rows);
    UmiArchiveWriteSigned(writer, (int64_t)value->auto_follow_active_task);
    UmiArchiveWriteSigned(writer, (int64_t)value->auto_open_review);
    UmiArchiveWriteSigned(writer, (int64_t)value->show_tool_arguments);
    UmiArchiveWriteSigned(writer, (int64_t)value->show_validation_output);
    UmiArchiveWriteSigned(writer, (int64_t)value->show_context_token_estimates);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiAiDeveloperPreferencesArchiveRead(UmiArchiveReader *reader, UmiAiDeveloperPreferences *value)
{
    value->diff_layout = (UmiAiDeveloperDiffLayout)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->diff_context_lines = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->visible_rows = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->auto_follow_active_task = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->auto_open_review = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->show_tool_arguments = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->show_validation_output = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->show_context_token_estimates = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiAiDeveloperPreferencesArchiveValidate(const UmiAiDeveloperPreferences *value)
{
    return umi_ai_developer_preferences_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ai_developer_preferences_archive_encode, umi_ai_developer_preferences_archive_decode,
    UmiAiDeveloperPreferences, UmiAiDeveloperPreferencesArchiveSchema, UmiAiDeveloperPreferencesArchiveBound, UmiAiDeveloperPreferencesArchiveWrite, UmiAiDeveloperPreferencesArchiveRead, UmiAiDeveloperPreferencesArchiveValidate)
