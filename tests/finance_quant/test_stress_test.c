#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/stress_test.h"

int main(void)
{
    UmiQuantStressTest value;
    CHECK(umi_quant_stress_test_init(&value, 1000000.0, 900000.0, 150000.0) == UMI_STATUS_OK);
    CHECK(umi_quant_stress_test_loss(&value) > 99999.0);
    return 0;
}
