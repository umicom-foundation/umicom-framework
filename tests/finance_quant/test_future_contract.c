#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/future_contract.h"

int main(void)
{
    UmiQuantFutureContract value;
    CHECK(umi_quant_future_contract_init(&value, 2.0, 50.0, 100.0, 101.5) == UMI_STATUS_OK);
    CHECK(umi_quant_future_contract_mark_to_market(&value) > 149.9 && umi_quant_future_contract_mark_to_market(&value) < 150.1);
    return 0;
}
