/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_result_model.c
 *
 * PURPOSE:
 *   Verify typed result-grid columns, rows, paging and selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/data/workbench/result_model.h"

int main(void)
{
    UmiDataResultModel *model =
        (UmiDataResultModel *)calloc(1U, sizeof(*model));
    const char *values[] = {"1", "Alice"};
    int nulls[] = {0, 0};
    assert(model != NULL);
    umi_data_result_model_init(model);
    assert(umi_data_result_model_add_column(
               model, "id", "integer", 0) == UMI_STATUS_OK);
    assert(umi_data_result_model_add_column(
               model, "name", "text", 1) == UMI_STATUS_OK);
    assert(umi_data_result_model_add_row(
               model, values, nulls, 2U, 10U) == UMI_STATUS_OK);
    assert(umi_data_result_model_set_page(model, 0U, 50U, 1U) ==
           UMI_STATUS_OK);
    assert(umi_data_result_model_select_row(model, 0U) == UMI_STATUS_OK);
    assert(strcmp(umi_data_result_model_row_at(model, 0U)->cells[1].text,
                  "Alice") == 0);
    free(model);
    return 0;
}
