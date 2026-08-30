/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_checkpoint.c
 *
 * PURPOSE:
 *   Implement the test valuation checkpoint behavior for
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

#include "umicom/finance/enterprise/valuation_checkpoint.h"

int main(void){UmiEnterpriseValuationCheckpoint c;CHECK(umi_enterprise_valuation_checkpoint_init(&c,"j",5U,10U,12U,1)==UMI_STATUS_OK);CHECK(c.completed_tasks==5U);return 0;}
