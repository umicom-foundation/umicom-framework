/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_database_schema_catalog.c
 *
 * PURPOSE:
 *   Implement the test database schema catalog behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/data/schema_catalog.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDatabaseSchemaCatalog *catalog;
    UmiDatabaseTableInfo table = {0};
    UmiDatabaseTableInfo result;
    UmiDatabaseColumnInfo column = {0};

    catalog = (UmiDatabaseSchemaCatalog *)malloc(sizeof(*catalog));
    assert(catalog != NULL);
    (void)strcpy(table.schema_name, "main");
    (void)strcpy(table.name, "orders");
    (void)strcpy(column.name, "order_id");
    (void)strcpy(column.type_name, "integer");
    column.primary_key = 1;
    assert(umi_database_table_add_column(&table, &column) == UMI_STATUS_OK);
    assert(umi_database_schema_catalog_init(catalog) == UMI_STATUS_OK);
    assert(umi_database_schema_catalog_upsert(catalog, &table) == UMI_STATUS_OK);
    assert(umi_database_schema_catalog_find(catalog, "main", "orders", &result) == UMI_STATUS_OK);
    assert(result.column_count == 1U && result.columns[0].primary_key);
    free(catalog);
    return 0;
}
