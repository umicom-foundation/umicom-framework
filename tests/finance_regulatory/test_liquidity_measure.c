#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/liquidity_measure.h"

int main(void)
{
    UmiLiquidityMeasure record;
    CHECK(umi_reg_liquidity_measure_init(&record, "measure_id-1", "Name", 1.25, (UmiCurrency){"USD"}) == UMI_STATUS_OK);
    CHECK(record.measure_id[0] != '\0');
    CHECK(record.name[0] != '\0');
    return 0;
}
