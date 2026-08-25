#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/withdrawal_policy.h"

int main(void)
{
    UmiDigitalWithdrawalPolicy value;
    CHECK(umi_digital_asset_withdrawal_policy_init(&value, "CUST-1", 1000000, 100000, 8, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_withdrawal_policy_valid(&value));
    return 0;
}
