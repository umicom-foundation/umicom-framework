/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reporting_checkpoint.c
 *
 * PURPOSE:
 *   Implement the test reporting checkpoint behavior for
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

#include "umicom/finance/regulatory/reporting_checkpoint.h"

int main(void)
{
    UmiReportingCheckpoint record;
    CHECK(umi_reg_reporting_checkpoint_init(&record, "checkpoint_id-1", "job_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.checkpoint_id[0] != '\0');
    CHECK(record.job_id[0] != '\0');
    return 0;
}
