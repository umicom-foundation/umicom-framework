/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_result_mapping.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the result mapping enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/result_mapping.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataResultMapping item;
    CHECK(umi_data_result_mapping_init(&item,"m1","Order","id",0U,UMI_DATA_VALUE_INTEGER) == UMI_STATUS_OK);
    CHECK(item.column_ordinal==0U);
    return 0;
}
