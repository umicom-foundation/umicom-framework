/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reporting_queue.c
 *
 * PURPOSE:
 *   Implement the test reporting queue behavior for
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

#include "umicom/finance/regulatory/reporting_queue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiReportingQueue q;UmiReportingQueueItem i;umi_reg_reporting_queue_init(&q);CHECK(umi_reg_reporting_queue_push(&q,"low",5U)==UMI_STATUS_OK);CHECK(umi_reg_reporting_queue_push(&q,"high",1U)==UMI_STATUS_OK);CHECK(umi_reg_reporting_queue_pop(&q,&i)==UMI_STATUS_OK);CHECK(strcmp(i.job_id,"high")==0);return 0; }
