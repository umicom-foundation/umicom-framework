/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_relation_mapping.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the relation mapping enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/relation_mapping.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataRelationMapping item;
    CHECK(umi_data_relation_mapping_init(&item,"Order.customer","Order","Customer","customer_id",false) == UMI_STATUS_OK);
    CHECK(!item.collection);
    return 0;
}
