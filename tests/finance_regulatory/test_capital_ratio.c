/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_capital_ratio.c
 *
 * PURPOSE:
 *   Implement the test capital ratio behavior for
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

#include "umicom/finance/regulatory/capital_ratio.h"

int main(void){ UmiCapitalRatio r; CHECK(umi_reg_capital_ratio_compute(&r,12.0,100.0)==UMI_STATUS_OK); CHECK(r.ratio>0.119&&r.ratio<0.121); return 0; }
