#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/token_balance.h"

int main(void)
{
    UmiDigitalTokenBalance value;
    CHECK(umi_digital_asset_token_balance_init(&value, "CUST-1", "ASSET-BTC", 100000000, 10000000, 8) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_token_balance_valid(&value));
    CHECK(value.available_units - value.reserved_units == 90000000);
    return 0;
}
