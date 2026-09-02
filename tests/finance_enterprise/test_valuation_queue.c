/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_queue.c
 *
 * PURPOSE:
 *   Implement the test valuation queue behavior for
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

#include "umicom/finance/enterprise/valuation_queue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterpriseValuationQueue q; UmiEnterpriseValuationJob a,b,o; umi_enterprise_valuation_queue_init(&q); CHECK(umi_enterprise_valuation_job_init(&a,"a","p",1,1)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_job_init(&b,"b","p",1,9)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_queue_push(&q,&a)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_queue_push(&q,&b)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_queue_pop(&q,&o)==UMI_STATUS_OK); CHECK(o.priority==9); return 0; }
