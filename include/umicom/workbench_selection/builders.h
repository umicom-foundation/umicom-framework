/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/builders.h
 *
 * PURPOSE:
 *   Build structured selections for Studio, Desk, Trader, TMS and future workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_BUILDERS_H
#define UMICOM_WORKBENCH_SELECTION_BUILDERS_H

#include "umicom/workbench_selection/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_selection_build_file(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *path,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_project(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *project_id,
    const char *root_path,
    const char *repository_id,
    const char *language_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_diagnostic(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *diagnostic_id,
    const char *path,
    uint32_t line,
    uint32_t column,
    const char *severity,
    const char *code,
    const char *message,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_source_control_change(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *project_id,
    const char *root_path,
    const char *repository_id,
    const char *branch,
    const char *selected_path,
    const char *change_kind,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_test(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *test_id,
    const char *suite_id,
    const char *outcome,
    const char *source_uri,
    uint64_t source_line,
    uint64_t duration_ms,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_ai_message(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *message_id,
    const char *conversation_id,
    const char *provider_id,
    const char *model_id,
    const char *evidence_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_instrument(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *instrument_id,
    const char *symbol,
    const char *venue,
    const char *currency,
    const char *asset_class,
    const char *contract_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_account(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *account_id,
    const char *organisation_id,
    const char *book_id,
    const char *currency,
    const char *account_type,
    const char *environment,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_build_trade(
    UmiWorkbenchSelection *selection,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *trade_id,
    const char *source_system,
    const char *product_type,
    const char *book_id,
    const char *counterparty_id,
    uint64_t version,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
