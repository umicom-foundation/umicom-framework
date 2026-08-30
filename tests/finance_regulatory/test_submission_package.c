/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_submission_package.c
 *
 * PURPOSE:
 *   Implement the test submission package behavior for
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

#include "umicom/finance/regulatory/submission_package.h"

int main(void)
{
    UmiSubmissionPackage record;
    CHECK(umi_reg_submission_package_init(&record, "package_id-1", "report_id-1", "Format", 1) == UMI_STATUS_OK);
    CHECK(record.package_id[0] != '\0');
    CHECK(record.report_id[0] != '\0');
    CHECK(record.format[0] != '\0');
    return 0;
}
