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
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiAiDeveloperDiffLayout {
    UMI_AI_DEVELOPER_DIFF_LAYOUT_UNIFIED = 0,
    UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE = 1
} UmiAiDeveloperDiffLayout;

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

void umi_ai_developer_preferences_init(
    UmiAiDeveloperPreferences *preferences);

UmiStatus umi_ai_developer_preferences_validate(
    const UmiAiDeveloperPreferences *preferences);

#ifdef __cplusplus
}
#endif
#endif
