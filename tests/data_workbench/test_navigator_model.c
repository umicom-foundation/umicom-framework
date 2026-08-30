/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_navigator_model.c
 *
 * PURPOSE:
 *   Verify connection/schema/table/column navigator projection and filtering.
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

#include "umicom/data/workbench/navigator_model.h"

int main(void)
{
    UmiDataServer *server = NULL;
    UmiDatabaseExplorer *explorer = NULL;
    UmiDataNavigatorModel *model = NULL;
    UmiDatabaseTableInfo table = {0};
    UmiDatabaseColumnInfo column = {0};
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    explorer = (UmiDatabaseExplorer *)calloc(1U, sizeof(*explorer));
    model = (UmiDataNavigatorModel *)calloc(1U, sizeof(*model));
    assert(explorer != NULL && model != NULL);
    assert(umi_database_explorer_init(explorer, server) == UMI_STATUS_OK);
    (void)strcpy(table.schema_name, "public");
    (void)strcpy(table.name, "orders");
    (void)strcpy(column.name, "order_id");
    (void)strcpy(column.type_name, "integer");
    column.primary_key = 1;
    assert(umi_database_table_add_column(&table, &column) == UMI_STATUS_OK);
    assert(umi_database_schema_catalog_upsert(&explorer->schema, &table) ==
           UMI_STATUS_OK);
    assert(umi_data_navigator_model_build(model, explorer) == UMI_STATUS_OK);
    assert(model->count >= 3U);
    assert(umi_data_navigator_model_set_filter(model, "order_id") ==
           UMI_STATUS_OK);
    assert(model->visible_count == 1U);
    assert(umi_data_navigator_model_select(model, 0U) == UMI_STATUS_OK);
    free(model);
    free(explorer);
    umi_data_server_destroy(server);
    return 0;
}
