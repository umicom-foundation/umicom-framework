/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_entity_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the entity descriptor enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/entity_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataEntityDescriptor item;
    CHECK(umi_data_entity_descriptor_init(&item,"Order","orders","id") == UMI_STATUS_OK);
    CHECK(strcmp(item.table_id,"orders")==0);
    return 0;
}
