/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_value_at_risk.c
 *
 * PURPOSE:
 *   Implement the test value at risk behavior for
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

#include "umicom/finance/enterprise/value_at_risk.h"

int main(void){ const double l[5]={1,5,2,4,3};double v=0;CHECK(umi_enterprise_value_at_risk_historical(l,5U,0.8,&v)==UMI_STATUS_OK);CHECK(v==5.0);return 0; }
