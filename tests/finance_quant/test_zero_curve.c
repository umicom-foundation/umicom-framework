#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/zero_curve.h"

int main(void)
{
    UmiQuantZeroCurve curve; double rate = 0.0;
    umi_quant_zero_curve_init(&curve);
    CHECK(umi_quant_zero_curve_add(&curve, 0, 0.02) == UMI_STATUS_OK);
    CHECK(umi_quant_zero_curve_add(&curve, 365, 0.04) == UMI_STATUS_OK);
    CHECK(umi_quant_zero_curve_rate(&curve, 182, &rate) == UMI_STATUS_OK);
    CHECK(rate > 0.029 && rate < 0.031);
    return 0;
}
