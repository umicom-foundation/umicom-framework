#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/quantity_conversion.h"

int main(void)
{
    UmiCommodityQuantityConversion conversion;
    int64_t output = 0;
    CHECK(umi_commodity_quantity_conversion_init(&conversion, "MT", "KG", 1000, 1) == UMI_STATUS_OK);
    CHECK(umi_commodity_quantity_conversion_apply(&conversion, 3, &output) == UMI_STATUS_OK);
    CHECK(output == 3000);
    return 0;
}
