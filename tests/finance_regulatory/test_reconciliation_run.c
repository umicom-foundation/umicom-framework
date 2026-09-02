/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reconciliation_run.c
 *
 * PURPOSE:
 *   Implement the test reconciliation run behavior for
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
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reconciliation_run.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiReconciliationRun r; CHECK(umi_reg_reconciliation_run_init(&r,"run")==UMI_STATUS_OK); CHECK(umi_reg_reconciliation_run_record(&r,10.0,10.01,0.02)==UMI_STATUS_OK); CHECK(r.matched==1U); return 0; }
