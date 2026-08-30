/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_worker_pool.c
 *
 * PURPOSE:
 *   Implement the test valuation worker pool behavior for
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

#include "umicom/finance/enterprise/valuation_worker_pool.h"

int main(void){ UmiEnterpriseValuationWorkerPool p; UmiEnterpriseValuationWorker a,b; umi_enterprise_valuation_worker_pool_init(&p); CHECK(umi_enterprise_valuation_worker_init(&a,"a",2U)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_init(&b,"b",2U)==UMI_STATUS_OK); a.active_tasks=1U; CHECK(umi_enterprise_valuation_worker_pool_add(&p,&a)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_pool_add(&p,&b)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_pool_select(&p)->worker_id[0]=='b'); return 0; }
