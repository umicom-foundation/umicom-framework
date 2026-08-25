/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_fingerprint.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema fingerprint enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_fingerprint.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSchemaSnapshot s; UmiDataSchemaTable t; uint64_t a,b; umi_data_schema_snapshot_init(&s); CHECK(umi_data_schema_table_init(&t,"orders","orders")==UMI_STATUS_OK); CHECK(umi_data_schema_snapshot_add(&s,&t)==UMI_STATUS_OK); a=umi_data_schema_fingerprint_compute(&s); b=umi_data_schema_fingerprint_compute(&s); CHECK(a!=0U); CHECK(a==b);
    return 0;
}
