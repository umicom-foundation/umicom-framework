/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_currency_pair.c
 *
 * PURPOSE:
 *   Exercise the currency pair financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/currency_pair.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCurrency a={{'E','U','R','\0'}},b={{'U','S','D','\0'}}; UmiCurrencyPair p,q; CHECK(umi_currency_pair_init(&p,a,b)==UMI_STATUS_OK); CHECK(umi_currency_pair_invert(&p,&q)==UMI_STATUS_OK); CHECK(strcmp(q.symbol,"USDEUR")==0);
    return 0;
}
