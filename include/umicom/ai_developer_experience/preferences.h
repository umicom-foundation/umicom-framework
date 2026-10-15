/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/preferences.h
 *
 * PURPOSE:
 *   Define reusable AI developer experience preferences independently of any
 *   toolkit settings dialog.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PREFERENCES_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PREFERENCES_H
#include "umicom/ai_developer_experience/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ai developer diff layout values accepted by this public contract.
 */
typedef enum UmiAiDeveloperDiffLayout {
    UMI_AI_DEVELOPER_DIFF_LAYOUT_UNIFIED = 0,
    UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE = 1
} UmiAiDeveloperDiffLayout;

/**
 * Represent the ai developer preferences data shared with callers of this public contract.
 */
typedef struct UmiAiDeveloperPreferences {
    UmiAiDeveloperDiffLayout diff_layout;
    size_t diff_context_lines;
    size_t visible_rows;
    int auto_follow_active_task;
    int auto_open_review;
    int show_tool_arguments;
    int show_validation_output;
    int show_context_token_estimates;
    uint64_t revision;
} UmiAiDeveloperPreferences;

/**
 * Initialise ai developer preferences from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_developer_preferences_init(
    UmiAiDeveloperPreferences *preferences);

/**
 * Check that ai developer preferences satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ai_developer_preferences_validate(
    const UmiAiDeveloperPreferences *preferences);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ai_developer_preferences_archive_encode(const UmiAiDeveloperPreferences *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ai_developer_preferences_archive_decode(const void *bytes, size_t byte_count,
    UmiAiDeveloperPreferences *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ai_developer_preferences_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAiDeveloperPreferences *expected, UmiAiDeveloperPreferences *value);


#ifdef __cplusplus
}
#endif
#endif
