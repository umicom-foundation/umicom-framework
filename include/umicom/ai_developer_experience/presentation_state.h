/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/presentation_state.h
 *
 * PURPOSE:
 *   Initialize and mutate the toolkit-neutral active pane/selection state used
 *   by Studio, web, Qt or headless review clients.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PRESENTATION_STATE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PRESENTATION_STATE_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

void umi_ai_developer_presentation_state_init(
    UmiAiDeveloperPresentationState *state);

UmiStatus umi_ai_developer_presentation_set_pane(
    UmiAiDeveloperPresentationState *state,
    UmiAiDeveloperPaneKind pane);

UmiStatus umi_ai_developer_presentation_set_chat(
    UmiAiDeveloperPresentationState *state,
    const char *chat_id);

UmiStatus umi_ai_developer_presentation_set_task(
    UmiAiDeveloperPresentationState *state,
    const char *task_id);

UmiStatus umi_ai_developer_presentation_set_approval(
    UmiAiDeveloperPresentationState *state,
    const char *approval_id);

UmiStatus umi_ai_developer_presentation_set_patch(
    UmiAiDeveloperPresentationState *state,
    const char *patch_id);

UmiStatus umi_ai_developer_presentation_set_diff(
    UmiAiDeveloperPresentationState *state,
    const char *path,
    size_t line_index);

#ifdef __cplusplus
}
#endif
#endif
