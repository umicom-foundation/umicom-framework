/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/builders.c
 *
 * PURPOSE:
 *   Implement strongly typed selection builders without parsing presentation strings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/builders.h"

#include <stdio.h>

static UmiStatus set_text_field(
    UmiWorkbenchSelection *selection,
    const char *name,
    const char *value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    if (selection == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_text(
        &field, value != NULL ? value : "");
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

static UmiStatus set_unsigned_field(
    UmiWorkbenchSelection *selection,
    const char *name,
    uint64_t value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_unsigned(&field, value);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

static UmiStatus initialise(
    UmiWorkbenchSelection *selection,
    UmiWorkbenchSelectionKind kind,
    UmiContextKind context_kind,
    const char *selection_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *subject_id,
    uint64_t timestamp_ms)
{
    UmiStatus status;
    if (selection == NULL || selection_id == NULL ||
        application_id == NULL || panel_id == NULL ||
        subject_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_selection_init(
        selection, kind, context_kind, selection_id);
    status = umi_workbench_selection_set_origin(
        selection, application_id, panel_id, workspace_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_subject(
        selection, subject_id, NULL);
    if (status != UMI_STATUS_OK) return status;
    selection->timestamp_ms = timestamp_ms;
    selection->state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_FILE,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        selection_id,
        application_id,
        panel_id,
        workspace_id,
        path,
        timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_location(
        selection, path, line, column, selection_length);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_workbench_selection_set_display_text(selection, path);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_PROJECT,
        UMI_CONTEXT_KIND_PROJECT,
        selection_id, application_id, panel_id, workspace_id,
        project_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_location(
        selection, root_path, 0U, 0U, 0U);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"repository-id",repository_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"language-id",language_id))!=UMI_STATUS_OK) return status;
    (void)umi_workbench_selection_set_display_text(selection, project_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_DIAGNOSTIC,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        selection_id, application_id, panel_id, workspace_id,
        diagnostic_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_location(
        selection, path, line, column, 0U);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"severity",severity))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"diagnostic-code",code))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"message",message))!=UMI_STATUS_OK) return status;
    (void)umi_workbench_selection_set_display_text(selection, message != NULL ? message : diagnostic_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_CHANGE,
        UMI_CONTEXT_KIND_PROJECT,
        selection_id, application_id, panel_id, workspace_id,
        project_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_location(
        selection, root_path, 0U, 0U, 0U);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"repository-id",repository_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"branch",branch))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"selected-path",selected_path))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"change-kind",change_kind))!=UMI_STATUS_OK) return status;
    (void)umi_workbench_selection_set_display_text(selection, selected_path != NULL ? selected_path : project_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_TEST,
        UMI_CONTEXT_KIND_SELECTION,
        selection_id, application_id, panel_id, workspace_id,
        test_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"suite-id",suite_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"outcome",outcome))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"source-uri",source_uri))!=UMI_STATUS_OK) return status;
    if ((status=set_unsigned_field(selection,"source-line",source_line))!=UMI_STATUS_OK) return status;
    if ((status=set_unsigned_field(selection,"duration-ms",duration_ms))!=UMI_STATUS_OK) return status;
    selection->unsigned_value = source_line;
    (void)umi_workbench_selection_set_display_text(selection, test_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_AI_MESSAGE,
        UMI_CONTEXT_KIND_SELECTION,
        selection_id, application_id, panel_id, workspace_id,
        message_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"conversation-id",conversation_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"provider-id",provider_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"model-id",model_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"evidence-id",evidence_id))!=UMI_STATUS_OK) return status;
    (void)umi_workbench_selection_set_display_text(selection, message_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_INSTRUMENT,
        UMI_CONTEXT_KIND_INSTRUMENT,
        selection_id, application_id, panel_id, NULL,
        instrument_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"symbol",symbol))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"venue",venue))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"currency",currency))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"asset-class",asset_class))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"contract-id",contract_id))!=UMI_STATUS_OK) return status;
    (void)umi_workbench_selection_set_display_text(selection, symbol != NULL ? symbol : instrument_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_ACCOUNT,
        UMI_CONTEXT_KIND_ACCOUNT,
        selection_id, application_id, panel_id, NULL,
        account_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"organisation-id",organisation_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"book-id",book_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"currency",currency))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"account-type",account_type))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"environment",environment))!=UMI_STATUS_OK) return status;
    (void)umi_workbench_selection_set_display_text(selection, account_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}

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
    uint64_t timestamp_ms)
{
    UmiStatus status = initialise(
        selection,
        UMI_WORKBENCH_SELECTION_TRADE,
        UMI_CONTEXT_KIND_TRADE,
        selection_id, application_id, panel_id, NULL,
        trade_id, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"source-system",source_system))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"product-type",product_type))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"book-id",book_id))!=UMI_STATUS_OK) return status;
    if ((status=set_text_field(selection,"counterparty-id",counterparty_id))!=UMI_STATUS_OK) return status;
    if ((status=set_unsigned_field(selection,"version",version))!=UMI_STATUS_OK) return status;
    selection->unsigned_value = version;
    (void)umi_workbench_selection_set_display_text(selection, trade_id);
    (void)umi_workbench_selection_refresh_hash(selection);
    return UMI_STATUS_OK;
}
