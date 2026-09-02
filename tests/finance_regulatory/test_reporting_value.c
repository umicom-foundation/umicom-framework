/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reporting_value.c
 *
 * PURPOSE:
 *   Implement the test reporting value behavior for
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

#include "umicom/finance/regulatory/reporting_value.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReportingValue record;
    CHECK(umi_reg_reporting_value_init(&record, "field_id-1", "source_id-1", 1.25, 1) == UMI_STATUS_OK);
    CHECK(record.field_id[0] != '\0');
    CHECK(record.source_id[0] != '\0');
    return 0;
}
