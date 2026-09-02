/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/runtime.c
 *
 * PURPOSE:
 *   Coordinate canonical Database Explorer execution with professional
 *   renderer-neutral Database, SQL and data-modelling workbench models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDataWorkbenchRuntime {
    UmiDatabaseExplorer *explorer;
    UmiDataWorkbenchConnectionModel connections;
    UmiDataNavigatorModel navigator;
    UmiDataSqlDocument sql;
    UmiDataResultModel *results;
    UmiDataQueryBuilder query_builder;
    UmiDataSchemaCompareModel schema_compare;
    UmiDataErModel er_model;
    UmiDataEditorModel data_editor;
    UmiDataTransferWorkbench transfers;
    UmiDataAdminModel admin;
    UmiDataWorkbenchPhase phase;
    UmiStatus last_status;
    char last_message[UMI_DATABASE_TEXT_CAPACITY];
    uint64_t revision;
};

/* Provide the finish operation used by this module and its client applications. */
static UmiStatus finish(
    UmiDataWorkbenchRuntime *runtime,
    UmiStatus status,
    UmiDataWorkbenchPhase phase,
    const char *success_message)
{
    runtime->last_status = status;
    runtime->phase = status == UMI_STATUS_OK ? phase : UMI_DATA_WORKBENCH_FAILED;
    (void)snprintf(runtime->last_message, sizeof(runtime->last_message), "%s",
                   status == UMI_STATUS_OK ? success_message
                                           : umi_status_text(status));
    runtime->revision += 1U;
    return status;
}

/*
 * Initialise data workbench runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_data_workbench_runtime_create(
    UmiDatabaseExplorer *explorer,
    UmiDataWorkbenchRuntime **out_runtime)
{
    UmiDataWorkbenchRuntime *runtime;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (explorer == NULL || explorer->server == NULL || out_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_runtime = NULL;
    runtime = (UmiDataWorkbenchRuntime *)calloc(1U, sizeof(*runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    runtime->results = (UmiDataResultModel *)calloc(1U, sizeof(*runtime->results));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime->results == NULL) {
        free(runtime);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    runtime->explorer = explorer;
    status = umi_data_workbench_connection_model_init(
        &runtime->connections, &explorer->connections);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_data_navigator_model_build(&runtime->navigator, explorer);
    }
    umi_data_result_model_init(runtime->results);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_data_query_builder_init(
            &runtime->query_builder, "workbench-query", "records");
    }
    umi_data_schema_compare_model_init(&runtime->schema_compare);
    umi_data_er_model_init(&runtime->er_model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_data_editor_model_init(
            &runtime->data_editor, "records", 1);
    }
    umi_data_transfer_workbench_init(&runtime->transfers);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < explorer->transfer_count;
         ++index) {
        status = umi_data_transfer_workbench_add(
            &runtime->transfers, &explorer->transfers[index]);
    }
    umi_data_admin_model_init(&runtime->admin);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_data_admin_model_refresh(
            &runtime->admin, explorer->server, NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(runtime->results);
        free(runtime);
        return status;
    }
    runtime->phase = UMI_DATA_WORKBENCH_IDLE;
    runtime->last_status = UMI_STATUS_OK;
    runtime->revision = 1U;
    (void)snprintf(runtime->last_message, sizeof(runtime->last_message), "%s",
                   "Database workbench ready.");
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by data workbench runtime so the same storage can be reused
 * safely.
 */
void umi_data_workbench_runtime_destroy(UmiDataWorkbenchRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return;
    free(runtime->results);
    free(runtime);
}

/*
 * Provide the data workbench runtime refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_workbench_runtime_refresh(
    UmiDataWorkbenchRuntime *runtime,
    const UmiDataPlatformService *service)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_workbench_connection_model_refresh(&runtime->connections);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_data_navigator_model_build(
            &runtime->navigator, runtime->explorer);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_data_admin_model_refresh(
            &runtime->admin, runtime->explorer->server, service);
    }
    return finish(runtime, status, UMI_DATA_WORKBENCH_BROWSING,
                  "Database workbench refreshed.");
}

/*
 * Provide the data workbench runtime open sql operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_workbench_runtime_open_sql(
    UmiDataWorkbenchRuntime *runtime,
    const char *document_id,
    const char *title,
    const char *dialect)
{
    UmiStatus status;
    UmiDatabaseConnectionProfile active;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_sql_document_init(
        &runtime->sql, document_id, title, dialect);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        umi_database_connection_registry_active(
            &runtime->explorer->connections, &active) == UMI_STATUS_OK) {
        status = umi_data_sql_document_bind_connection(
            &runtime->sql, active.id);
    }
    return finish(runtime, status, UMI_DATA_WORKBENCH_EDITING_SQL,
                  "SQL document opened.");
}

/*
 * Perform data workbench runtime through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_data_workbench_runtime_execute(
    UmiDataWorkbenchRuntime *runtime)
{
    char statement[UMI_DATABASE_SQL_CAPACITY];
    UmiDatabaseQueryResult result;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_sql_document_statement(
        &runtime->sql, statement, sizeof(statement));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return finish(runtime, status, UMI_DATA_WORKBENCH_FAILED,
                      "SQL statement unavailable.");
    }
    runtime->phase = UMI_DATA_WORKBENCH_EXECUTING;
    runtime->revision += 1U;
    status = umi_database_query_session_execute(
        &runtime->explorer->queries, statement, &result);
    (void)umi_data_result_model_set_execution(runtime->results, &result);
    return finish(runtime, status, UMI_DATA_WORKBENCH_SHOWING_RESULTS,
                  "SQL statement completed.");
}

/*
 * Provide the data workbench runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_workbench_runtime_snapshot(
    const UmiDataWorkbenchRuntime *runtime,
    UmiDataWorkbenchRuntimeSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    out_snapshot->phase = runtime->phase;
    out_snapshot->visible_connections = runtime->connections.visible_count;
    out_snapshot->navigator_nodes = runtime->navigator.visible_count;
    out_snapshot->result_rows = runtime->results->row_count;
    out_snapshot->schema_changes = runtime->schema_compare.change_count;
    out_snapshot->er_entities = runtime->er_model.entity_count;
    out_snapshot->pending_edits = runtime->data_editor.edit_count;
    out_snapshot->transfers = runtime->transfers.plan_count;
    out_snapshot->transaction_active =
        runtime->explorer->transaction.active;
    out_snapshot->can_execute = runtime->sql.text_length > 0U;
    out_snapshot->can_apply_edits = runtime->data_editor.ready_to_apply &&
        (!runtime->data_editor.transaction_required ||
         runtime->explorer->transaction.active);
    out_snapshot->healthy = runtime->admin.healthy;
    out_snapshot->last_status = runtime->last_status;
    (void)snprintf(out_snapshot->last_message,
                   sizeof(out_snapshot->last_message), "%s",
                   runtime->last_message);
    out_snapshot->revision = runtime->revision;
    return UMI_STATUS_OK;
}

#define ACCESSOR(name, type, member)                                           \
    type *name(UmiDataWorkbenchRuntime *runtime)                               \
    {                                                                          \
        return runtime != NULL ? &runtime->member : NULL;                      \
    }

ACCESSOR(umi_data_workbench_runtime_connections,
         UmiDataWorkbenchConnectionModel, connections)
ACCESSOR(umi_data_workbench_runtime_navigator,
         UmiDataNavigatorModel, navigator)
ACCESSOR(umi_data_workbench_runtime_sql,
         UmiDataSqlDocument, sql)
ACCESSOR(umi_data_workbench_runtime_query_builder,
         UmiDataQueryBuilder, query_builder)
ACCESSOR(umi_data_workbench_runtime_schema_compare,
         UmiDataSchemaCompareModel, schema_compare)
ACCESSOR(umi_data_workbench_runtime_er_model,
         UmiDataErModel, er_model)
ACCESSOR(umi_data_workbench_runtime_data_editor,
         UmiDataEditorModel, data_editor)
ACCESSOR(umi_data_workbench_runtime_transfers,
         UmiDataTransferWorkbench, transfers)
ACCESSOR(umi_data_workbench_runtime_admin,
         UmiDataAdminModel, admin)

#undef ACCESSOR

/*
 * Provide the data workbench runtime results operation used by this module and its client
 * applications.
 */
UmiDataResultModel *umi_data_workbench_runtime_results(
    UmiDataWorkbenchRuntime *runtime)
{
    return runtime != NULL ? runtime->results : NULL;
}
