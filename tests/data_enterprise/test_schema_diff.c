/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_diff.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema diff enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_diff.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSchemaSnapshot a,b; UmiDataSchemaTable t; UmiDataSchemaDiff d;
    umi_data_schema_snapshot_init(&a); umi_data_schema_snapshot_init(&b); CHECK(umi_data_schema_table_init(&t,"orders","orders")==UMI_STATUS_OK); CHECK(umi_data_schema_snapshot_add(&a,&t)==UMI_STATUS_OK); CHECK(umi_data_schema_snapshot_add(&b,&t)==UMI_STATUS_OK); CHECK(umi_data_schema_diff_compute(&a,&b,&d)==UMI_STATUS_OK); CHECK(d.unchanged_tables==1U); CHECK(d.compatibility==UMI_DATA_COMPATIBLE);
    return 0;
}
