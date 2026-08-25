#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/bond_yield.h"

int main(void)
{
    UmiQuantBondYield value;
    CHECK(umi_quant_bond_yield_init(&value, 950.0, 1000.0, 50.0, 5.0) == UMI_STATUS_OK);
    CHECK(umi_quant_bond_yield_approximate_yield(&value) > 0.061 && umi_quant_bond_yield_approximate_yield(&value) < 0.062);
    return 0;
}
