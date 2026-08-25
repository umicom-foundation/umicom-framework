/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_table.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema table enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_table.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSchemaTable table; UmiDataSchemaColumn col, found;
    CHECK(umi_data_schema_table_init(&table,"orders","orders")==UMI_STATUS_OK);
    CHECK(umi_data_schema_column_init(&col,"orders.id","id",UMI_DATA_VALUE_INTEGER,0U,false)==UMI_STATUS_OK);
    CHECK(umi_data_schema_table_add_column(&table,&col)==UMI_STATUS_OK);
    CHECK(umi_data_schema_table_add_column(&table,&col)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_data_schema_table_find_column(&table,"orders.id",&found)==UMI_STATUS_OK);
    CHECK(found.kind==UMI_DATA_VALUE_INTEGER);
    return 0;
}
