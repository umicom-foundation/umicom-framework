#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reporting_calendar.h"

int main(void)
{
    UmiReportingCalendar record;
    CHECK(umi_reg_reporting_calendar_init(&record, "calendar_id-1", "authority_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.calendar_id[0] != '\0');
    CHECK(record.authority_id[0] != '\0');
    return 0;
}
