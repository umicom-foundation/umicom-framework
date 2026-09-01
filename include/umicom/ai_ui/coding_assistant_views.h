/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_ui/coding_assistant_views.h
 *
 * PURPOSE:
 *   Project coding-assistant, repository-context and patch-review state into
 *   toolkit-neutral Framework view models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These factories return ordinary data models.  A GTK4, Qt6, Wt, web or test
 * adapter can render the same coding workflow without moving policy into UI.
 */
#ifndef UMICOM_AI_UI_CODING_ASSISTANT_VIEWS_H
#define UMICOM_AI_UI_CODING_ASSISTANT_VIEWS_H

#include "umicom/ai/coding_assistant.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_ui_coding_assistant_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    UmiUiViewModel **out_view);
UmiStatus umi_ai_ui_coding_repository_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    UmiUiViewModel **out_view);
UmiStatus umi_ai_ui_coding_patch_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
