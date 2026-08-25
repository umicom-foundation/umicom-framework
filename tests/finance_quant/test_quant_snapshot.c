#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/quant_snapshot.h"

int main(void)
{
    UmiQuantQuantSnapshot value;
    CHECK(umi_quant_quant_snapshot_init(&value, 2U, 3U, 4U, 5U) == UMI_STATUS_OK);
    CHECK(umi_quant_quant_snapshot_asset_count(&value) == 14.0);
    return 0;
}
