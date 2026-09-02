/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_black76.c
 *
 * PURPOSE:
 *   Implement the test black76 behavior for
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

#include <math.h>
#include "umicom/finance/quant/black76.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    double price=0.0; CHECK(umi_quant_black76_price(100.0,100.0,0.20,1.0,1.0,UMI_QUANT_CALL,&price)==UMI_STATUS_OK); CHECK(price>7.9&&price<8.1);
    return 0;
}
