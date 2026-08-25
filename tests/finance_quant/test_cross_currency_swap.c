#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/cross_currency_swap.h"

int main(void)
{
    UmiQuantCrossCurrencySwap value;
    CHECK(umi_quant_cross_currency_swap_init(&value, 1100000.0, 1000000.0, 1.10) == UMI_STATUS_OK);
    CHECK(umi_quant_cross_currency_swap_principal_mismatch(&value) > -0.1 && umi_quant_cross_currency_swap_principal_mismatch(&value) < 0.1);
    return 0;
}
