/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_schema_compare.c
 *
 * PURPOSE:
 *   Verify per-table schema changes enrich the canonical aggregate diff.
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

#include "umicom/data/workbench/schema_compare.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataSchemaSnapshot before;
    UmiDataSchemaSnapshot after;
    UmiDataSchemaTable table;
    UmiDataSchemaCompareModel *model =
        (UmiDataSchemaCompareModel *)calloc(1U, sizeof(*model));
    assert(model != NULL);
    umi_data_schema_snapshot_init(&before);
    umi_data_schema_snapshot_init(&after);
    assert(umi_data_schema_table_init(&table, "customers", "customers") ==
           UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&before, &table) == UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&after, &table) == UMI_STATUS_OK);
    assert(umi_data_schema_table_init(&table, "orders", "orders") ==
           UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&after, &table) == UMI_STATUS_OK);
    assert(umi_data_schema_compare_model_build(model, &before, &after) ==
           UMI_STATUS_OK);
    assert(model->summary.added_tables == 1U);
    assert(model->change_count == 2U);
    assert(umi_data_schema_compare_model_select(model, 1U) == UMI_STATUS_OK);
    free(model);
    return 0;
}
