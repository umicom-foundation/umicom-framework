#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/commodity_descriptor.h"

int main(void)
{
    UmiCommodityDescriptor value;
    UmiCurrency currency = {{'U','S','D','\0'}};
    CHECK(umi_commodity_commodity_descriptor_init(&value, "CMD-WTI", "West Texas Intermediate", "WTI", UMI_COMMODITY_KIND_ENERGY, &currency, true) == UMI_STATUS_OK);
    CHECK(umi_commodity_commodity_descriptor_valid(&value));
    CHECK(value.physical_delivery);
    return 0;
}
