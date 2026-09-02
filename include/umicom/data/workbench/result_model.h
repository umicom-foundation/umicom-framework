/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/result_model.h
 *
 * PURPOSE:
 *   Represent bounded typed query-result grids, paging and selection for GTK4,
 *   Qt6, web and headless workbench frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_RESULT_MODEL_H
#define UMICOM_DATA_WORKBENCH_RESULT_MODEL_H

#include "umicom/data/query_session.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data result column data shared with callers of this public contract.
 */
typedef struct UmiDataResultColumn {
    char name[UMI_DATABASE_NAME_CAPACITY];
    char type_name[UMI_DATABASE_NAME_CAPACITY];
    int nullable;
} UmiDataResultColumn;

/**
 * Represent the data result cell data shared with callers of this public contract.
 */
typedef struct UmiDataResultCell {
    char text[UMI_DATA_WORKBENCH_CELL_CAPACITY];
    int null_value;
    int truncated;
} UmiDataResultCell;

/**
 * Represent the data result row data shared with callers of this public contract.
 */
typedef struct UmiDataResultRow {
    UmiDataResultCell cells[UMI_DATA_WORKBENCH_MAX_RESULT_COLUMNS];
    size_t cell_count;
    uint64_t source_row;
} UmiDataResultRow;

/**
 * Represent the data result model data shared with callers of this public contract.
 */
typedef struct UmiDataResultModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDataResultColumn columns[UMI_DATA_WORKBENCH_MAX_RESULT_COLUMNS];
    UmiDataResultRow rows[UMI_DATA_WORKBENCH_MAX_RESULT_ROWS];
    size_t column_count;
    size_t row_count;
    size_t total_rows;
    size_t page_offset;
    size_t page_size;
    size_t selected_row;
    UmiDatabaseQueryResult execution;
    int truncated;
    uint64_t revision;
} UmiDataResultModel;

/**
 * Initialise data result model from caller-provided values so later operations receive a
 * known state.
 */
void umi_data_result_model_init(UmiDataResultModel *model);
/**
 * Provide the data result model set execution operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_set_execution(
    UmiDataResultModel *model,
    const UmiDatabaseQueryResult *result);
/**
 * Provide the data result model add column operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_add_column(
    UmiDataResultModel *model,
    const char *name,
    const char *type_name,
    int nullable);
/**
 * Provide the data result model add row operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_add_row(
    UmiDataResultModel *model,
    const char *const *values,
    const int *null_values,
    size_t value_count,
    uint64_t source_row);
/**
 * Provide the data result model set page operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_set_page(
    UmiDataResultModel *model,
    size_t offset,
    size_t page_size,
    size_t total_rows);
/**
 * Provide the data result model select row operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_select_row(
    UmiDataResultModel *model,
    size_t row_index);
/**
 * Find data result model row while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDataResultRow *umi_data_result_model_row_at(
    const UmiDataResultModel *model,
    size_t row_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_RESULT_MODEL_H */
