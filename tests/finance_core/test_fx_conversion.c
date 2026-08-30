/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_fx_conversion.c
 *
 * PURPOSE:
 *   Exercise the fx conversion financial-core contract.
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
#include "umicom/finance/core/fx_conversion.h"

int main(void)
{
    UmiCurrency a={{'E','U','R','\0'}},b={{'U','S','D','\0'}}; UmiCurrencyPair p; UmiExchangeRate r; UmiMoney in={10000,2U,{{'E','U','R','\0'}}},out={0}; CHECK(umi_currency_pair_init(&p,a,b)==UMI_STATUS_OK); CHECK(umi_exchange_rate_init(&r,&p,1.2,1)==UMI_STATUS_OK); CHECK(umi_fx_conversion_base_to_quote(in,&r,2U,&out)==UMI_STATUS_OK); CHECK(out.minor_units==12000);
    return 0;
}
