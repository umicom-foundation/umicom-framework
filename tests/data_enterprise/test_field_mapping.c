/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_field_mapping.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the field mapping enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/field_mapping.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataFieldMapping item;
    CHECK(umi_data_field_mapping_init(&item,"Order.id","Order","id","id",UMI_DATA_VALUE_INTEGER) == UMI_STATUS_OK);
    CHECK(item.kind==UMI_DATA_VALUE_INTEGER);
    return 0;
}
