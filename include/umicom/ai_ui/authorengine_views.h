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

/*
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

/**
 * Initialise ai ui authorengine overview view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_ui_authorengine_overview_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise ai ui chat view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_ui_chat_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise ai ui runtime catalogue view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_ui_runtime_catalogue_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise ai ui context view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_ui_context_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise ai ui sessions view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_ui_sessions_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise ai ui privacy view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_ui_privacy_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
