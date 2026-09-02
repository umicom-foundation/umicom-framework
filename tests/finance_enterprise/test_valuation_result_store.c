/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_result_store.c
 *
 * PURPOSE:
 *   Implement the test valuation result store behavior for
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

#include "umicom/finance/enterprise/valuation_result_store.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterpriseValuationResultStore s; UmiEnterpriseValuationResultRecord r={{0},{0},10.0,UMI_STATUS_OK}; umi_enterprise_valuation_result_store_init(&s); CHECK(umi_quant_copy_text(r.job_id,sizeof r.job_id,"j")==UMI_STATUS_OK); CHECK(umi_quant_copy_text(r.task_id,sizeof r.task_id,"t")==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_result_store_append(&s,&r)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_result_store_job_pv(&s,"j")==10.0); return 0; }
