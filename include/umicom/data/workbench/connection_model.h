/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/connection_model.h
 *
 * PURPOSE:
 *   Project canonical connection profiles into a filterable workbench model
 *   while retaining credential references inside the security boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_CONNECTION_MODEL_H
#define UMICOM_DATA_WORKBENCH_CONNECTION_MODEL_H

#include "umicom/data/connection_profile.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data workbench connection model data shared with callers of this public
 * contract.
 */
typedef struct UmiDataWorkbenchConnectionModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDatabaseConnectionRegistry *registry;
    size_t visible_indices[UMI_DATABASE_MAX_CONNECTIONS];
    size_t visible_count;
    size_t ready_count;
    size_t failed_count;
    char filter[UMI_DATABASE_NAME_CAPACITY];
    uint64_t revision;
} UmiDataWorkbenchConnectionModel;

/**
 * Initialise data workbench connection model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_data_workbench_connection_model_init(
    UmiDataWorkbenchConnectionModel *model,
    UmiDatabaseConnectionRegistry *registry);
/**
 * Provide the data workbench connection model refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_data_workbench_connection_model_refresh(
    UmiDataWorkbenchConnectionModel *model);
/**
 * Provide the data workbench connection model set filter operation used by this module and
 * its client applications.
 */
UmiStatus umi_data_workbench_connection_model_set_filter(
    UmiDataWorkbenchConnectionModel *model,
    const char *filter);
/**
 * Provide the data workbench connection model activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_data_workbench_connection_model_activate(
    UmiDataWorkbenchConnectionModel *model,
    const char *connection_id);
/**
 * Find data workbench connection model visible while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDatabaseConnectionProfile *
umi_data_workbench_connection_model_visible_at(
    const UmiDataWorkbenchConnectionModel *model,
    size_t visible_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_CONNECTION_MODEL_H */
