#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reporting_job.h"

int main(void)
{
    UmiReportingJob record;
    CHECK(umi_reg_reporting_job_init(&record, "job_id-1", "report_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.job_id[0] != '\0');
    CHECK(record.report_id[0] != '\0');
    return 0;
}
