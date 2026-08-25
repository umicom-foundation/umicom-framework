#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/pricing_request.h"

int main(void)
{
    UmiQuantPricingRequest value;
    CHECK(umi_quant_pricing_request_init(&value, 3, 2.0, 1000000.0, 20000) == UMI_STATUS_OK);
    CHECK(umi_quant_pricing_request_gross_exposure(&value) > 1999999.0);
    return 0;
}
