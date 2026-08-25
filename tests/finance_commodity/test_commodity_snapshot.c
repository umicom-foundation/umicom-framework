#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/commodity_snapshot.h"

int main(void)
{
    UmiCommoditySnapshot snapshot;
    umi_commodity_commodity_snapshot_init(&snapshot, 1000);
    CHECK(umi_commodity_commodity_snapshot_valid(&snapshot));
    CHECK(snapshot.revision == 1U);
    return 0;
}
