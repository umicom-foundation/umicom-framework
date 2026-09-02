/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_liquidity_ratio.c
 *
 * PURPOSE:
 *   Implement the test liquidity ratio behavior for
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

#include "umicom/finance/regulatory/liquidity_ratio.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiLiquidityRatio r; CHECK(umi_reg_liquidity_ratio_compute(&r,120.0,100.0)==UMI_STATUS_OK); CHECK(r.ratio>1.19&&r.ratio<1.21); return 0; }
