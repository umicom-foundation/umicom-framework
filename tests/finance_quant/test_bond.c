/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_bond.c
 *
 * PURPOSE:
 *   Implement the test bond behavior for
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

#include "umicom/finance/quant/bond.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantBond value;
    CHECK(umi_quant_bond_init(&value, 1000.0, 0.06, 2, 1825) == UMI_STATUS_OK);
    CHECK(umi_quant_bond_coupon_payment(&value) > 29.9 && umi_quant_bond_coupon_payment(&value) < 30.1);
    return 0;
}
