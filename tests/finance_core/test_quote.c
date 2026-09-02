/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_quote.c
 *
 * PURPOSE:
 *   Exercise the quote financial-core contract.
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
#include "umicom/finance/core/quote.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiMarketDataKey k; UmiFinancialPrice p; UmiQuote q; CHECK(umi_market_data_key_init(&k,"K","Key","SRC",1U)==UMI_STATUS_OK); CHECK(umi_price_init(&p,1.0,2U)==UMI_STATUS_OK); CHECK(umi_quote_init(&q,&k,UMI_QUOTE_MID,p,1)==UMI_STATUS_OK);
    return 0;
}
