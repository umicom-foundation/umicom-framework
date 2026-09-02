/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_cost.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query cost enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_cost.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataQueryPlan p; UmiDataQueryCost c; CHECK(umi_data_query_plan_init(&p,"q1","orders")==UMI_STATUS_OK); CHECK(umi_data_query_plan_shape(&p,1U,3U,2U,1U,100U)==UMI_STATUS_OK); CHECK(umi_data_query_cost_estimate(&p,5.0,&c)==UMI_STATUS_OK); CHECK(c.expensive);
    return 0;
}
