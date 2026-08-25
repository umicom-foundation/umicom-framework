#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/transaction_book.h"

int main(void)
{
    UmiDigitalTransactionBook catalogue;
    UmiDigitalAssetTransaction item;
    umi_digital_asset_transaction_book_init(&catalogue);
    CHECK(umi_digital_asset_transaction_init(&item, "TX-1", "BTC", "from", "to", 1000, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transaction_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transaction_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_transaction_book_find(&catalogue, "TX-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
