/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_join.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query join enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_join.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataQueryJoin item;
    CHECK(umi_data_query_join_init(&item,"j1","orders","customers","orders.customer_id=customers.id",false) == UMI_STATUS_OK);
    CHECK(!item.outer_join);
    return 0;
}
