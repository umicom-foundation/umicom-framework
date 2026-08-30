/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_identifier.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema identifier enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_identifier.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSchemaIdentifier item;
    CHECK(umi_data_schema_identifier_init(&item, "main", "public", "orders") == UMI_STATUS_OK);
    CHECK(strcmp(item.name,"orders")==0);
    return 0;
}
