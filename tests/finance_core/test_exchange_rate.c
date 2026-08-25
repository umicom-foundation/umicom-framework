/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_exchange_rate.c
 *
 * PURPOSE:
 *   Exercise the exchange rate financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/exchange_rate.h"

int main(void)
{
    UmiCurrency a={{'E','U','R','\0'}},b={{'U','S','D','\0'}}; UmiCurrencyPair p; UmiExchangeRate r; CHECK(umi_currency_pair_init(&p,a,b)==UMI_STATUS_OK); CHECK(umi_exchange_rate_init(&r,&p,1.2,1)==UMI_STATUS_OK);
    return 0;
}
