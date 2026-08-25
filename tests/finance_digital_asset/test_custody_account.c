#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/custody_account.h"

int main(void)
{
    UmiDigitalCustodyAccount value;
    UmiFinancialId owner = {{"PARTY-1"}};
    CHECK(umi_digital_asset_custody_account_init(&value, "CUST-1", &owner, "WALLET-1", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_custody_account_valid(&value));
    CHECK(value.segregated);
    return 0;
}
