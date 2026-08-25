#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/fra.h"

int main(void)
{
    UmiQuantFra value;
    CHECK(umi_quant_fra_init(&value, 1000000.0, 0.04, 0.05, 90, 360) == UMI_STATUS_OK);
    CHECK(umi_quant_fra_undiscounted_payoff(&value) > 2499.0 && umi_quant_fra_undiscounted_payoff(&value) < 2501.0);
    return 0;
}
