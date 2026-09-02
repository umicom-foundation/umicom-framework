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

/**
 * Represent the data workbench runtime data shared with callers of this public contract.
 */
typedef struct UmiDataWorkbenchRuntime UmiDataWorkbenchRuntime;

/**
 * Represent the data workbench runtime snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise data workbench runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_data_workbench_runtime_create(
    UmiDatabaseExplorer *explorer,
    UmiDataWorkbenchRuntime **out_runtime);
/**
 * Release or reset state held by data workbench runtime so the same storage can be reused
 * safely.
 */
void umi_data_workbench_runtime_destroy(UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_workbench_runtime_refresh(
    UmiDataWorkbenchRuntime *runtime,
    const UmiDataPlatformService *service);
/**
 * Provide the data workbench runtime open sql operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_workbench_runtime_open_sql(
    UmiDataWorkbenchRuntime *runtime,
    const char *document_id,
    const char *title,
    const char *dialect);
/**
 * Perform data workbench runtime through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_data_workbench_runtime_execute(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_workbench_runtime_snapshot(
    const UmiDataWorkbenchRuntime *runtime,
    UmiDataWorkbenchRuntimeSnapshot *out_snapshot);

/**
 * Provide the data workbench runtime connections operation used by this module and its
 * client applications.
 */
UmiDataWorkbenchConnectionModel *umi_data_workbench_runtime_connections(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime navigator operation used by this module and its
 * client applications.
 */
UmiDataNavigatorModel *umi_data_workbench_runtime_navigator(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime sql operation used by this module and its client
 * applications.
 */
UmiDataSqlDocument *umi_data_workbench_runtime_sql(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime results operation used by this module and its client
 * applications.
 */
UmiDataResultModel *umi_data_workbench_runtime_results(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime query builder operation used by this module and its
 * client applications.
 */
UmiDataQueryBuilder *umi_data_workbench_runtime_query_builder(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime schema compare operation used by this module and its
 * client applications.
 */
UmiDataSchemaCompareModel *umi_data_workbench_runtime_schema_compare(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime er model operation used by this module and its client
 * applications.
 */
UmiDataErModel *umi_data_workbench_runtime_er_model(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime data editor operation used by this module and its
 * client applications.
 */
UmiDataEditorModel *umi_data_workbench_runtime_data_editor(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime transfers operation used by this module and its
 * client applications.
 */
UmiDataTransferWorkbench *umi_data_workbench_runtime_transfers(
    UmiDataWorkbenchRuntime *runtime);
/**
 * Provide the data workbench runtime admin operation used by this module and its client
 * applications.
 */
UmiDataAdminModel *umi_data_workbench_runtime_admin(
    UmiDataWorkbenchRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_RUNTIME_H */
