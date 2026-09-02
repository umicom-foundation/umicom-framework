/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/result_model.c
 *
 * PURPOSE:
 *   Implement bounded result-grid columns, rows, paging and selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/result_model.h"

#include <string.h>

/*
 * Initialise data result model from caller-provided values so later operations receive a
 * known state.
 */
void umi_data_result_model_init(UmiDataResultModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    model->page_size = UMI_DATA_WORKBENCH_MAX_RESULT_ROWS;
    model->revision = 1U;
}

/*
 * Provide the data result model set execution operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_set_execution(
    UmiDataResultModel *model,
    const UmiDatabaseQueryResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->execution = *result;
    model->total_rows = result->row_count;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data result model add column operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_add_column(
    UmiDataResultModel *model,
    const char *name,
    const char *type_name,
    int nullable)
{
    UmiDataResultColumn *column;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || name == NULL || name[0] == '\0' || type_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->column_count >= UMI_DATA_WORKBENCH_MAX_RESULT_COLUMNS) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    column = &model->columns[model->column_count];
    status = umi_data_workbench_copy_text(
        column->name, sizeof(column->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        column->type_name, sizeof(column->type_name), type_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    column->nullable = nullable != 0;
    model->column_count += 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data result model add row operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_add_row(
    UmiDataResultModel *model,
    const char *const *values,
    const int *null_values,
    size_t value_count,
    uint64_t source_row)
{
    UmiDataResultRow *row;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || values == NULL || value_count != model->column_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->row_count >= UMI_DATA_WORKBENCH_MAX_RESULT_ROWS) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    row = &model->rows[model->row_count];
    (void)memset(row, 0, sizeof(*row));
    row->cell_count = value_count;
    row->source_row = source_row;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < value_count; ++index) {
        const char *value = values[index] != NULL ? values[index] : "";
        UmiStatus status = umi_data_workbench_copy_text(
            row->cells[index].text, sizeof(row->cells[index].text), value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
            size_t last = sizeof(row->cells[index].text) - 1U;
            (void)memcpy(row->cells[index].text, value, last);
            row->cells[index].text[last] = '\0';
            row->cells[index].truncated = 1;
            model->truncated = 1;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) {
            return status;
        }
        row->cells[index].null_value =
            null_values != NULL && null_values[index] != 0;
    }
    model->row_count += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (model->total_rows < model->row_count) model->total_rows = model->row_count;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data result model set page operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_set_page(
    UmiDataResultModel *model,
    size_t offset,
    size_t page_size,
    size_t total_rows)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || page_size == 0U ||
        page_size > UMI_DATA_WORKBENCH_MAX_RESULT_ROWS || offset > total_rows) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->page_offset = offset;
    model->page_size = page_size;
    model->total_rows = total_rows;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data result model select row operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_result_model_select_row(
    UmiDataResultModel *model,
    size_t row_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (row_index >= model->row_count) return UMI_STATUS_NOT_FOUND;
    model->selected_row = row_index;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find data result model row while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDataResultRow *umi_data_result_model_row_at(
    const UmiDataResultModel *model,
    size_t row_index)
{
    return model != NULL && row_index < model->row_count
        ? &model->rows[row_index] : NULL;
}
