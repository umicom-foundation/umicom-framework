/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_report_section.c
 *
 * PURPOSE:
 *   Implement the test report section behavior for
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

#include "umicom/finance/regulatory/report_section.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReportSection record;
    CHECK(umi_reg_report_section_init(&record, "section_id-1", "template_id-1", "Title", 1) == UMI_STATUS_OK);
    CHECK(record.section_id[0] != '\0');
    CHECK(record.template_id[0] != '\0');
    CHECK(record.title[0] != '\0');
    return 0;
}
