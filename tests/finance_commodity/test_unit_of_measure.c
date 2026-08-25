#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/unit_of_measure.h"

int main(void)
{
    UmiCommodityUnitOfMeasure value;
    CHECK(umi_commodity_unit_of_measure_init(&value, "BBL", "VOLUME", 158987, 1000) == UMI_STATUS_OK);
    CHECK(umi_commodity_unit_of_measure_valid(&value));
    return 0;
}
