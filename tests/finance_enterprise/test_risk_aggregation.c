/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_risk_aggregation.c
 *
 * PURPOSE:
 *   Implement the test risk aggregation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/risk_aggregation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ const double r[2]={3.0,4.0},w[2]={1.0,1.0}; double x=0.0; CHECK(umi_enterprise_risk_aggregate_rss(r,w,2U,&x)==UMI_STATUS_OK); CHECK(x>4.999&&x<5.001); return 0; }
