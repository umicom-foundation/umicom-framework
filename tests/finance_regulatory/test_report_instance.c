#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/report_instance.h"

int main(void)
{
    UmiReportInstance r;
    CHECK(umi_reg_report_instance_init(&r, "rep-1", "tmpl-1") == UMI_STATUS_OK);
    CHECK(umi_reg_report_instance_record_fact(&r, 1) == UMI_STATUS_OK);
    CHECK(umi_reg_report_instance_complete(&r, 1U) == 1);
    return 0;
}
