/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_parameter.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query parameter enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_parameter.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataQueryParameter item;
    CHECK(umi_data_query_parameter_init(&item,"p1","status",UMI_DATA_VALUE_TEXT,"OPEN",false) == UMI_STATUS_OK);
    CHECK(strcmp(item.value,"OPEN")==0);
    return 0;
}
