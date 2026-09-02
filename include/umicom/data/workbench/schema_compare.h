/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/schema_compare.h
 *
 * PURPOSE:
 *   Enrich the canonical aggregate schema diff with selectable per-table
 *   changes for DDL review and migration planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_SCHEMA_COMPARE_H
#define UMICOM_DATA_WORKBENCH_SCHEMA_COMPARE_H

#include "umicom/data/enterprise/schema_diff.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data schema table change data shared with callers of this public contract.
 */
typedef struct UmiDataSchemaTableChange {
    char table_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char before_name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char after_name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    UmiDataSchemaChangeKind kind;
    size_t before_columns;
    size_t after_columns;
    int selected;
} UmiDataSchemaTableChange;

/**
 * Represent the data schema compare model data shared with callers of this public
 * contract.
 */
typedef struct UmiDataSchemaCompareModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDataSchemaSnapshot before;
    UmiDataSchemaSnapshot after;
    UmiDataSchemaDiff summary;
    UmiDataSchemaTableChange changes[UMI_DATA_WORKBENCH_MAX_SCHEMA_CHANGES];
    size_t change_count;
    size_t selected_index;
    int ready;
    int truncated;
    uint64_t revision;
} UmiDataSchemaCompareModel;

/**
 * Initialise data schema compare model from caller-provided values so later operations
 * receive a known state.
 */
void umi_data_schema_compare_model_init(UmiDataSchemaCompareModel *model);
/**
 * Provide the data schema compare model build operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_schema_compare_model_build(
    UmiDataSchemaCompareModel *model,
    const UmiDataSchemaSnapshot *before,
    const UmiDataSchemaSnapshot *after);
/**
 * Provide the data schema compare model select operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_schema_compare_model_select(
    UmiDataSchemaCompareModel *model,
    size_t change_index);
/**
 * Find data schema compare model change while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDataSchemaTableChange *umi_data_schema_compare_model_change_at(
    const UmiDataSchemaCompareModel *model,
    size_t change_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_SCHEMA_COMPARE_H */
