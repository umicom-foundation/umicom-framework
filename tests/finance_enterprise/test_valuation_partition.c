/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_partition.c
 *
 * PURPOSE:
 *   Implement the test valuation partition behavior for
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

#include "umicom/finance/enterprise/valuation_partition.h"

int main(void){ UmiEnterpriseValuationPartition p; CHECK(umi_enterprise_valuation_partition_init(&p,0U,0U,10U,25U)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_partition_init(&p,0U,20U,10U,25U)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
