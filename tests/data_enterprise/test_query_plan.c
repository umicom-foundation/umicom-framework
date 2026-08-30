/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query plan enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataQueryPlan p; CHECK(umi_data_query_plan_init(&p,"q1","orders")==UMI_STATUS_OK); CHECK(umi_data_query_plan_shape(&p,2U,4U,1U,1U,100U)==UMI_STATUS_OK); CHECK(umi_data_query_plan_validate(&p)==UMI_STATUS_OK);
    return 0;
}
