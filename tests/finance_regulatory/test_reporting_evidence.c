/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reporting_evidence.c
 *
 * PURPOSE:
 *   Implement the test reporting evidence behavior for
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

#include "umicom/finance/regulatory/reporting_evidence.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReportingEvidence record;
    CHECK(umi_reg_reporting_evidence_init(&record, "evidence_id-1", "source_id-1", "locator value", 1) == UMI_STATUS_OK);
    CHECK(record.evidence_id[0] != '\0');
    CHECK(record.source_id[0] != '\0');
    CHECK(record.locator[0] != '\0');
    return 0;
}
