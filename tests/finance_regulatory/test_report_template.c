/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_report_template.c
 *
 * PURPOSE:
 *   Implement the test report template behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/report_template.h"

int main(void)
{
    UmiReportTemplate record;
    CHECK(umi_reg_report_template_init(&record, "template_id-1", "taxonomy_id-1", "Name", 1) == UMI_STATUS_OK);
    CHECK(record.template_id[0] != '\0');
    CHECK(record.taxonomy_id[0] != '\0');
    CHECK(record.name[0] != '\0');
    return 0;
}
