#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/inventory_book.h"

int main(void)
{
    UmiCommodityInventoryBook catalogue;
    UmiCommodityStorageInventory item;
    umi_commodity_inventory_book_init(&catalogue);
    CHECK(umi_commodity_storage_inventory_init(&item, "FAC-1", "CMD-WTI", 1000, 0, "BBL") == UMI_STATUS_OK);
    CHECK(umi_commodity_inventory_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_inventory_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_inventory_book_find(&catalogue, "FAC-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
