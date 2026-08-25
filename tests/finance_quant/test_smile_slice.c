#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/smile_slice.h"

int main(void)
{
    UmiQuantSmileSlice value;
    CHECK(umi_quant_smile_slice_init(&value, 90.0, 0.25, 110.0, 0.20) == UMI_STATUS_OK);
    CHECK(umi_quant_smile_slice_slope(&value) < 0.0);
    return 0;
}
