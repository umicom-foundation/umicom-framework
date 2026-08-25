#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/custody_book.h"

int main(void)
{
    UmiDigitalCustodyBook catalogue;
    UmiDigitalCustodyAccount item;
    umi_digital_asset_custody_book_init(&catalogue);
    { UmiFinancialId owner = {{"PARTY-1"}}; CHECK(umi_digital_asset_custody_account_init(&item, "CUST-1", &owner, "WALLET-1", true) == UMI_STATUS_OK); }
    CHECK(umi_digital_asset_custody_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_custody_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_custody_book_find(&catalogue, "CUST-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
