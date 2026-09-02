/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/navigator_model.h
 *
 * PURPOSE:
 *   Build a renderer-neutral connection/schema/table/column navigation tree
 *   from the authoritative Database Explorer catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_NAVIGATOR_MODEL_H
#define UMICOM_DATA_WORKBENCH_NAVIGATOR_MODEL_H

#include "umicom/data/database_explorer.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data navigator node data shared with callers of this public contract.
 */
typedef struct UmiDataNavigatorNode {
    uint32_t struct_size;
    uint32_t api_version;
    char node_id[UMI_DATABASE_ID_CAPACITY];
    char parent_id[UMI_DATABASE_ID_CAPACITY];
    char label[UMI_DATABASE_NAME_CAPACITY];
    char detail[UMI_DATABASE_NAME_CAPACITY];
    UmiDataNavigatorNodeKind kind;
    size_t depth;
    size_t source_index;
    int expanded;
    int selected;
} UmiDataNavigatorNode;

/**
 * Represent the data navigator model data shared with callers of this public contract.
 */
typedef struct UmiDataNavigatorModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDataNavigatorNode nodes[UMI_DATA_WORKBENCH_MAX_NAVIGATOR_NODES];
    size_t visible_indices[UMI_DATA_WORKBENCH_MAX_NAVIGATOR_NODES];
    size_t count;
    size_t visible_count;
    size_t selected_index;
    char filter[UMI_DATABASE_NAME_CAPACITY];
    int truncated;
    uint64_t revision;
} UmiDataNavigatorModel;

/**
 * Initialise data navigator model from caller-provided values so later operations receive
 * a known state.
 */
void umi_data_navigator_model_init(UmiDataNavigatorModel *model);
/**
 * Provide the data navigator model build operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_navigator_model_build(
    UmiDataNavigatorModel *model,
    const UmiDatabaseExplorer *explorer);
/**
 * Provide the data navigator model set filter operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_navigator_model_set_filter(
    UmiDataNavigatorModel *model,
    const char *filter);
/**
 * Provide the data navigator model select operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_navigator_model_select(
    UmiDataNavigatorModel *model,
    size_t visible_index);
/**
 * Provide the data navigator model set expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_navigator_model_set_expanded(
    UmiDataNavigatorModel *model,
    const char *node_id,
    int expanded);
/**
 * Find data navigator model visible while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDataNavigatorNode *umi_data_navigator_model_visible_at(
    const UmiDataNavigatorModel *model,
    size_t visible_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_NAVIGATOR_MODEL_H */
