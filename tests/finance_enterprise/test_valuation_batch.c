/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_batch.c
 *
 * PURPOSE:
 *   Implement the test valuation batch behavior for
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

#include "umicom/finance/enterprise/valuation_batch.h"

int main(void){ UmiEnterpriseValuationBatch b; UmiEnterpriseValuationTask t; CHECK(umi_enterprise_valuation_batch_init(&b,"b")==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_task_init(&t,"t","j","p",1.0)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_batch_add(&b,&t)==UMI_STATUS_OK); CHECK(b.count==1U); return 0; }
