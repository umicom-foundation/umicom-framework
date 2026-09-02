/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/presentation_state.h
 *
 * PURPOSE:
 *   Initialize and mutate the toolkit-neutral active pane/selection state used
 *   by Studio, web, Qt or headless review clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PRESENTATION_STATE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PRESENTATION_STATE_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise ai developer presentation state from caller-provided values so later
 * operations receive a known state.
 */
void umi_ai_developer_presentation_state_init(
    UmiAiDeveloperPresentationState *state);

/**
 * Provide the ai developer presentation set pane operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_pane(
    UmiAiDeveloperPresentationState *state,
    UmiAiDeveloperPaneKind pane);

/**
 * Provide the ai developer presentation set chat operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_chat(
    UmiAiDeveloperPresentationState *state,
    const char *chat_id);

/**
 * Provide the ai developer presentation set task operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_task(
    UmiAiDeveloperPresentationState *state,
    const char *task_id);

/**
 * Provide the ai developer presentation set approval operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_approval(
    UmiAiDeveloperPresentationState *state,
    const char *approval_id);

/**
 * Provide the ai developer presentation set patch operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_patch(
    UmiAiDeveloperPresentationState *state,
    const char *patch_id);

/**
 * Provide the ai developer presentation set diff operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_diff(
    UmiAiDeveloperPresentationState *state,
    const char *path,
    size_t line_index);

#ifdef __cplusplus
}
#endif
#endif
