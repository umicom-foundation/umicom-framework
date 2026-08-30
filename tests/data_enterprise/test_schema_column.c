/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_column.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema column enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_column.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSchemaColumn item;
    CHECK(umi_data_schema_column_init(&item,"order.id","id",UMI_DATA_VALUE_INTEGER,0U,false) == UMI_STATUS_OK);
    CHECK(item.nullable == false);
    return 0;
}
