/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_index.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema index enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_index.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSchemaIndex item;
    CHECK(umi_data_schema_index_init(&item,"orders.pk","orders","id",true) == UMI_STATUS_OK);
    CHECK(item.unique);
    return 0;
}
