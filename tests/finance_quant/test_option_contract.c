#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/option_contract.h"

int main(void)
{
    UmiQuantOptionContract value;
    CHECK(umi_quant_option_contract_init(&value, 10.0, 100.0, 105.0, 1) == UMI_STATUS_OK);
    CHECK(umi_quant_option_contract_intrinsic(&value) > 49.9 && umi_quant_option_contract_intrinsic(&value) < 50.1);
    return 0;
}
