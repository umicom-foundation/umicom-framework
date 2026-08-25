/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_operation.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data operation enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_operation.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataOperation item;
    CHECK(umi_data_data_operation_init(&item,"op1","s1","query",10U,5U) == UMI_STATUS_OK);
    CHECK(item.cancellable);
    return 0;
}
