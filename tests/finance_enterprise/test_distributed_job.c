/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_distributed_job.c
 *
 * PURPOSE:
 *   Implement the test distributed job behavior for
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

#include "umicom/finance/enterprise/distributed_job.h"

int main(void){UmiEnterpriseDistributedJob j;CHECK(umi_enterprise_distributed_job_init(&j,"j","valuation",5,1U)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_job_retry(&j)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_job_retry(&j)==UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
