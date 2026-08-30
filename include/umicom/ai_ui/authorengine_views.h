/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_ui/authorengine_views.h
 *
 * PURPOSE:
 *   Project AuthorEngine Integration v2 state into toolkit-neutral view models
 *   for GTK4, Qt, Wt, web, headless tests and future Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These functions expose labels, rows, counters and action identifiers only.
 * They never create GTK widgets and never own the integration service.
 */
#ifndef UMICOM_AI_UI_AUTHOR_ENGINE_VIEWS_H
#define UMICOM_AI_UI_AUTHOR_ENGINE_VIEWS_H

#include "umicom/ai/authorengine_service.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_UI_VISIBLE_ROWS 32U

UmiStatus umi_ai_ui_authorengine_overview_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
UmiStatus umi_ai_ui_runtime_catalogue_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
UmiStatus umi_ai_ui_context_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
UmiStatus umi_ai_ui_sessions_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
UmiStatus umi_ai_ui_privacy_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
