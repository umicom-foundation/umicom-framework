/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/builders.h
 *
 * PURPOSE:
 *   Build normalised events for project, editor, diagnostics, source control, testing, AI and trading interactions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_BUILDERS_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_BUILDERS_H
#include "umicom/workbench_context_event/event.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_context_event_build_project(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *workspace_id,const char *project_id,
    const char *root_path,const char *repository_id,const char *language_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_editor_location(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *workspace_id,const char *file_path,
    const char *symbol,uint32_t line,uint32_t column,uint32_t selection_length,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_diagnostic(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *workspace_id,const char *file_path,
    uint32_t line,uint32_t column,const char *severity,
    const char *diagnostic_code,const char *message,uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_source_control(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *workspace_id,const char *project_id,
    const char *root_path,const char *repository_id,const char *branch,
    const char *path,const char *change_kind,uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_test(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *workspace_id,const char *test_id,
    const char *suite_id,const char *outcome,const char *source_uri,
    uint64_t source_line,uint64_t duration_ms,uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_ai(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *workspace_id,const char *message_id,
    const char *conversation_id,const char *provider_id,const char *model_id,
    const char *evidence_id,uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_instrument(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *instrument_id,const char *symbol,
    const char *venue,const char *currency,const char *asset_class,
    const char *contract_id,uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_account(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *account_id,const char *organisation_id,
    const char *book_id,const char *currency,const char *account_type,
    const char *environment,uint64_t timestamp_ms);

UmiStatus umi_workbench_context_event_build_trade(
    UmiWorkbenchContextEvent *event,
    const char *event_id,const char *source_id,const char *application_id,
    const char *panel_id,const char *trade_id,const char *source_system,
    const char *product_type,const char *book_id,const char *counterparty_id,
    uint64_t version,uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
