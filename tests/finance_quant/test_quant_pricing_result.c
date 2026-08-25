#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/quant_pricing_result.h"

int main(void)
{
    UmiQuantQuantPricingResult value;
    CHECK(umi_quant_quant_pricing_result_init(&value, 100.0, 99.0, 1.5) == UMI_STATUS_OK);
    CHECK(umi_quant_quant_pricing_result_dirty_price(&value) > 100.49 && umi_quant_quant_pricing_result_dirty_price(&value) < 100.51);
    return 0;
}
