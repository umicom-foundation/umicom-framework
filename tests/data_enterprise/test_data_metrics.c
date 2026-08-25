/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_metrics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data metrics enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_metrics.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataMetrics m={0}; m.queries=10U; m.query_failures=1U; m.transactions=4U; m.transaction_rollbacks=1U; CHECK(umi_data_metrics_refresh(&m)==UMI_STATUS_OK); CHECK(m.query_success_percent==90.0); CHECK(m.transaction_commit_percent==75.0);
    return 0;
}
