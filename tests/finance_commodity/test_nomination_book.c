#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/nomination_book.h"

int main(void)
{
    UmiCommodityNominationBook catalogue;
    UmiCommodityNomination item;
    umi_commodity_nomination_book_init(&catalogue);
    CHECK(umi_commodity_nomination_init(&item, "NOM-1", "CTR-1", 250, 0, "MT", 1000, 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_nomination_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_nomination_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_nomination_book_find(&catalogue, "NOM-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
