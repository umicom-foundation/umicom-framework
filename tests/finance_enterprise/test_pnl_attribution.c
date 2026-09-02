/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_pnl_attribution.c
 *
 * PURPOSE:
 *   Implement the test pnl attribution behavior for
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

#include "umicom/finance/enterprise/pnl_attribution.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEnterprisePnlAttribution a;CHECK(umi_enterprise_pnl_attribution_calculate(&a,10.0,5.0,2.0,1.0)==UMI_STATUS_OK);CHECK(a.residual==2.0);return 0;}
