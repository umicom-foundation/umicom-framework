#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/fee_quote.h"

int main(void)
{
    UmiDigitalFeeQuote value;
    CHECK(umi_digital_asset_fee_quote_init(&value, "BTC", 1200, 8, "BTC", 5000) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_fee_quote_valid(&value));
    return 0;
}
