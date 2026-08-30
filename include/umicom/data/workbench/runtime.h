/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/runtime.h
 *
 * PURPOSE:
 *   Coordinate canonical Database Explorer services and professional SQL,
 *   result, schema, ER, edit, transfer and administration workbench models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_RUNTIME_H
#define UMICOM_DATA_WORKBENCH_RUNTIME_H

#include "umicom/data/workbench/admin_model.h"
#include "umicom/data/workbench/connection_model.h"
#include "umicom/data/workbench/data_editor.h"
#include "umicom/data/workbench/er_model.h"
#include "umicom/data/workbench/navigator_model.h"
#include "umicom/data/workbench/query_builder.h"
#include "umicom/data/workbench/result_model.h"
#include "umicom/data/workbench/schema_compare.h"
#include "umicom/data/workbench/sql_document.h"
#include "umicom/data/workbench/transfer_workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataWorkbenchRuntime UmiDataWorkbenchRuntime;

typedef struct UmiDataWorkbenchRuntimeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDataWorkbenchPhase phase;
    size_t visible_connections;
    size_t navigator_nodes;
    size_t result_rows;
    size_t schema_changes;
    size_t er_entities;
    size_t pending_edits;
    size_t transfers;
    int transaction_active;
    int can_execute;
    int can_apply_edits;
    int healthy;
    UmiStatus last_status;
    char last_message[UMI_DATABASE_TEXT_CAPACITY];
    uint64_t revision;
} UmiDataWorkbenchRuntimeSnapshot;

UmiStatus umi_data_workbench_runtime_create(
    UmiDatabaseExplorer *explorer,
    UmiDataWorkbenchRuntime **out_runtime);
void umi_data_workbench_runtime_destroy(UmiDataWorkbenchRuntime *runtime);
UmiStatus umi_data_workbench_runtime_refresh(
    UmiDataWorkbenchRuntime *runtime,
    const UmiDataPlatformService *service);
UmiStatus umi_data_workbench_runtime_open_sql(
    UmiDataWorkbenchRuntime *runtime,
    const char *document_id,
    const char *title,
    const char *dialect);
UmiStatus umi_data_workbench_runtime_execute(
    UmiDataWorkbenchRuntime *runtime);
UmiStatus umi_data_workbench_runtime_snapshot(
    const UmiDataWorkbenchRuntime *runtime,
    UmiDataWorkbenchRuntimeSnapshot *out_snapshot);

UmiDataWorkbenchConnectionModel *umi_data_workbench_runtime_connections(
    UmiDataWorkbenchRuntime *runtime);
UmiDataNavigatorModel *umi_data_workbench_runtime_navigator(
    UmiDataWorkbenchRuntime *runtime);
UmiDataSqlDocument *umi_data_workbench_runtime_sql(
    UmiDataWorkbenchRuntime *runtime);
UmiDataResultModel *umi_data_workbench_runtime_results(
    UmiDataWorkbenchRuntime *runtime);
UmiDataQueryBuilder *umi_data_workbench_runtime_query_builder(
    UmiDataWorkbenchRuntime *runtime);
UmiDataSchemaCompareModel *umi_data_workbench_runtime_schema_compare(
    UmiDataWorkbenchRuntime *runtime);
UmiDataErModel *umi_data_workbench_runtime_er_model(
    UmiDataWorkbenchRuntime *runtime);
UmiDataEditorModel *umi_data_workbench_runtime_data_editor(
    UmiDataWorkbenchRuntime *runtime);
UmiDataTransferWorkbench *umi_data_workbench_runtime_transfers(
    UmiDataWorkbenchRuntime *runtime);
UmiDataAdminModel *umi_data_workbench_runtime_admin(
    UmiDataWorkbenchRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_RUNTIME_H */
