#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/valuation_adjustment.h"

int main(void)
{
    UmiQuantValuationAdjustment value;
    CHECK(umi_quant_valuation_adjustment_init(&value, 1000000.0, 0.02, 0.6, 0.95) == UMI_STATUS_OK);
    CHECK(umi_quant_valuation_adjustment_amount(&value) > 11399.0 && umi_quant_valuation_adjustment_amount(&value) < 11401.0);
    return 0;
}
