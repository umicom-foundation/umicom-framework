#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/balance_book.h"

int main(void)
{
    UmiDigitalBalanceBook catalogue;
    UmiDigitalTokenBalance item;
    umi_digital_asset_balance_book_init(&catalogue);
    CHECK(umi_digital_asset_token_balance_init(&item, "CUST-1", "ASSET-BTC", 1000, 0, 8) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_balance_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_balance_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_balance_book_find(&catalogue, "CUST-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
