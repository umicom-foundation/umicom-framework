#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/storage_inventory.h"

int main(void)
{
    UmiCommodityStorageInventory inventory;
    CHECK(umi_commodity_storage_inventory_init(&inventory, "FAC-1", "CMD-WTI", 1000, 0, "BBL") == UMI_STATUS_OK);
    CHECK(umi_commodity_storage_inventory_reserve(&inventory, 400) == UMI_STATUS_OK);
    CHECK(umi_commodity_storage_inventory_free_units(&inventory) == 600);
    CHECK(umi_commodity_storage_inventory_reserve(&inventory, 700) == UMI_STATUS_UNAVAILABLE);
    CHECK(umi_commodity_storage_inventory_release(&inventory, 100) == UMI_STATUS_OK);
    return 0;
}
