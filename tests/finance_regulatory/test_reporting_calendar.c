/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reporting_calendar.c
 *
 * PURPOSE:
 *   Implement the test reporting calendar behavior for
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

#include "umicom/finance/regulatory/reporting_calendar.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReportingCalendar record;
    CHECK(umi_reg_reporting_calendar_init(&record, "calendar_id-1", "authority_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.calendar_id[0] != '\0');
    CHECK(record.authority_id[0] != '\0');
    return 0;
}
