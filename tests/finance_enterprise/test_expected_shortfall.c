/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_expected_shortfall.c
 *
 * PURPOSE:
 *   Implement the test expected shortfall behavior for
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

#include "umicom/finance/enterprise/expected_shortfall.h"

int main(void){ const double l[5]={1,5,2,4,3};double es=0;CHECK(umi_enterprise_expected_shortfall_historical(l,5U,0.6,&es)==UMI_STATUS_OK);CHECK(es==4.5);return 0; }
