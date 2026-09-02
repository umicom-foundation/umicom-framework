/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_distributed_job_queue.c
 *
 * PURPOSE:
 *   Implement the test distributed job queue behavior for
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

#include "umicom/finance/enterprise/distributed_job_queue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEnterpriseDistributedJobQueue q;UmiEnterpriseDistributedJob a,b,o;umi_enterprise_distributed_job_queue_init(&q);CHECK(umi_enterprise_distributed_job_init(&a,"a","x",1,0U)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_job_init(&b,"b","x",10,0U)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_job_queue_push(&q,&a)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_job_queue_push(&q,&b)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_job_queue_pop(&q,&o)==UMI_STATUS_OK);CHECK(o.priority==10);return 0;}
