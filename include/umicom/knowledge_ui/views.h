/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge_ui/views.h
 * PURPOSE: Project knowledge state into toolkit-neutral view models.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* View models contain data and actions, never toolkit-specific widgets. */
#ifndef UMICOM_KNOWLEDGE_UI_VIEWS_H
#define UMICOM_KNOWLEDGE_UI_VIEWS_H

#include "umicom/knowledge/service.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise knowledge ui overview view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_knowledge_ui_overview_view_create(
    const char *view_id, UmiKnowledgeService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise knowledge ui collections view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_knowledge_ui_collections_view_create(
    const char *view_id, UmiKnowledgeService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise knowledge ui sources view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_knowledge_ui_sources_view_create(
    const char *view_id, UmiKnowledgeService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise knowledge ui results view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_knowledge_ui_results_view_create(
    const char *view_id, UmiKnowledgeService *service,
    const UmiKnowledgeMatch *matches, size_t match_count,
    UmiUiViewModel **out_view);
/**
 * Initialise knowledge ui source view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_knowledge_ui_source_view_create(
    const char *view_id, const UmiKnowledgeMatch *match,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif
#endif
