#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/loss_allowance.h"

int main(void)
{
    UmiCommodityLossAllowance value;
    CHECK(umi_commodity_loss_allowance_init(&value, "CTR-1", 25) == UMI_STATUS_OK);
    CHECK(umi_commodity_loss_allowance_valid(&value));
    return 0;
}
