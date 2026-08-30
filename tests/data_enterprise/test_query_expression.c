/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_expression.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query expression enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_expression.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataQueryExpression item;
    CHECK(umi_data_query_expression_init(&item,"e1","status","=","OPEN") == UMI_STATUS_OK);
    CHECK(item.parameterized);
    return 0;
}
