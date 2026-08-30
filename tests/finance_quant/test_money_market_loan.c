/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_money_market_loan.c
 *
 * PURPOSE:
 *   Implement the test money market loan behavior for
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

#include "umicom/finance/quant/money_market_loan.h"

int main(void)
{
    UmiQuantMoneyMarketLoan value;
    CHECK(umi_quant_money_market_loan_init(&value, 500000.0, 0.06, 90, 360) == UMI_STATUS_OK);
    CHECK(umi_quant_money_market_loan_interest(&value) > 7499.0 && umi_quant_money_market_loan_interest(&value) < 7501.0);
    return 0;
}
