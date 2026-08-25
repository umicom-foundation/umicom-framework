#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/credit_default_swap.h"

int main(void)
{
    UmiQuantCreditDefaultSwap value;
    CHECK(umi_quant_credit_default_swap_init(&value, 1000000.0, 0.02, 0.40) == UMI_STATUS_OK);
    CHECK(umi_quant_credit_default_swap_expected_loss(&value) > 11999.0 && umi_quant_credit_default_swap_expected_loss(&value) < 12001.0);
    return 0;
}
