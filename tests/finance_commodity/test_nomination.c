#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/nomination.h"

int main(void)
{
    UmiCommodityNomination value;
    CHECK(umi_commodity_nomination_init(&value, "NOM-1", "CTR-1", 250, 0, "MT", 1000, 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_nomination_valid(&value));
    return 0;
}
