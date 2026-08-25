#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/forward_curve.h"

int main(void)
{
    double forward = 0.0;
    CHECK(umi_quant_forward_curve_rate(0.03, 365, 0.04, 730, &forward) == UMI_STATUS_OK);
    CHECK(forward > 0.049 && forward < 0.052);
    return 0;
}
