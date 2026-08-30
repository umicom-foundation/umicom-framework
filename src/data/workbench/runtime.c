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

UmiStatus umi_data_workbench_runtime_create(
    UmiDatabaseExplorer *explorer,
    UmiDataWorkbenchRuntime **out_runtime)
{
    UmiDataWorkbenchRuntime *runtime;
    UmiStatus status;
    size_t index;
    if (explorer == NULL || explorer->server == NULL || out_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_runtime = NULL;
    runtime = (UmiDataWorkbenchRuntime *)calloc(1U, sizeof(*runtime));
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    runtime->results = (UmiDataResultModel *)calloc(1U, sizeof(*runtime->results));
    if (runtime->results == NULL) {
        free(runtime);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    runtime->explorer = explorer;
    status = umi_data_workbench_connection_model_init(
        &runtime->connections, &explorer->connections);
    if (status == UMI_STATUS_OK) {
        status = umi_data_navigator_model_build(&runtime->navigator, explorer);
    }
    umi_data_result_model_init(runtime->results);
    if (status == UMI_STATUS_OK) {
        status = umi_data_query_builder_init(
            &runtime->query_builder, "workbench-query", "records");
    }
    umi_data_schema_compare_model_init(&runtime->schema_compare);
    umi_data_er_model_init(&runtime->er_model);
    if (status == UMI_STATUS_OK) {
        status = umi_data_editor_model_init(
            &runtime->data_editor, "records", 1);
    }
    umi_data_transfer_workbench_init(&runtime->transfers);
    for (index = 0U;
         status == UMI_STATUS_OK && index < explorer->transfer_count;
         ++index) {
        status = umi_data_transfer_workbench_add(
            &runtime->transfers, &explorer->transfers[index]);
    }
    umi_data_admin_model_init(&runtime->admin);
    if (status == UMI_STATUS_OK) {
        status = umi_data_admin_model_refresh(
            &runtime->admin, explorer->server, NULL);
    }
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

void umi_data_workbench_runtime_destroy(UmiDataWorkbenchRuntime *runtime)
{
    if (runtime == NULL) return;
    free(runtime->results);
    free(runtime);
}

UmiStatus umi_data_workbench_runtime_refresh(
    UmiDataWorkbenchRuntime *runtime,
    const UmiDataPlatformService *service)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_workbench_connection_model_refresh(&runtime->connections);
    if (status == UMI_STATUS_OK) {
        status = umi_data_navigator_model_build(
            &runtime->navigator, runtime->explorer);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_data_admin_model_refresh(
            &runtime->admin, runtime->explorer->server, service);
    }
    return finish(runtime, status, UMI_DATA_WORKBENCH_BROWSING,
                  "Database workbench refreshed.");
}

UmiStatus umi_data_workbench_runtime_open_sql(
    UmiDataWorkbenchRuntime *runtime,
    const char *document_id,
    const char *title,
    const char *dialect)
{
    UmiStatus status;
    UmiDatabaseConnectionProfile active;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_sql_document_init(
        &runtime->sql, document_id, title, dialect);
    if (status == UMI_STATUS_OK &&
        umi_database_connection_registry_active(
            &runtime->explorer->connections, &active) == UMI_STATUS_OK) {
        status = umi_data_sql_document_bind_connection(
            &runtime->sql, active.id);
    }
    return finish(runtime, status, UMI_DATA_WORKBENCH_EDITING_SQL,
                  "SQL document opened.");
}

UmiStatus umi_data_workbench_runtime_execute(
    UmiDataWorkbenchRuntime *runtime)
{
    char statement[UMI_DATABASE_SQL_CAPACITY];
    UmiDatabaseQueryResult result;
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_sql_document_statement(
        &runtime->sql, statement, sizeof(statement));
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

UmiStatus umi_data_workbench_runtime_snapshot(
    const UmiDataWorkbenchRuntime *runtime,
    UmiDataWorkbenchRuntimeSnapshot *out_snapshot)
{
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

UmiDataResultModel *umi_data_workbench_runtime_results(
    UmiDataWorkbenchRuntime *runtime)
{
    return runtime != NULL ? runtime->results : NULL;
}
