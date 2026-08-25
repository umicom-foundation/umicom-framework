#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reporting_period.h"

int main(void)
{
    UmiReportingPeriod record;
    CHECK(umi_reg_reporting_period_init(&record, "period_id-1", 1, 1, 1) == UMI_STATUS_OK);
    CHECK(record.period_id[0] != '\0');
    return 0;
}
