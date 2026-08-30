/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_worker.c
 *
 * PURPOSE:
 *   Implement the test valuation worker behavior for
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

#include "umicom/finance/enterprise/valuation_worker.h"

int main(void){ UmiEnterpriseValuationWorker w; CHECK(umi_enterprise_valuation_worker_init(&w,"w",1U)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_acquire(&w)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_acquire(&w)==UMI_STATUS_BUSY); CHECK(umi_enterprise_valuation_worker_release(&w)==UMI_STATUS_OK); return 0; }
