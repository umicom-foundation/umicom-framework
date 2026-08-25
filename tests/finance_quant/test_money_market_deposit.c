#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/money_market_deposit.h"

int main(void)
{
    UmiQuantMoneyMarketDeposit value;
    CHECK(umi_quant_money_market_deposit_init(&value, 1000000.0, 0.05, 180, 360) == UMI_STATUS_OK);
    CHECK(umi_quant_money_market_deposit_interest(&value) > 24999.0 && umi_quant_money_market_deposit_interest(&value) < 25001.0);
    return 0;
}
