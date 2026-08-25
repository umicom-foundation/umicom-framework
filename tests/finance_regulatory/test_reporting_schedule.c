#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reporting_schedule.h"

int main(void)
{
    UmiReportingSchedule record;
    CHECK(umi_reg_reporting_schedule_init(&record, "schedule_id-1", "template_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.schedule_id[0] != '\0');
    CHECK(record.template_id[0] != '\0');
    return 0;
}
