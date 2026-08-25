/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_foreign_key.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema foreign key enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_foreign_key.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSchemaForeignKey item;
    CHECK(umi_data_schema_foreign_key_init(&item,"fk.order.customer","orders","customer_id","customers","id") == UMI_STATUS_OK);
    CHECK(strcmp(item.target_table,"customers")==0);
    return 0;
}
