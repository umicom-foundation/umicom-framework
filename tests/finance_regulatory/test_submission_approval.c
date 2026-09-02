/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_submission_approval.c
 *
 * PURPOSE:
 *   Implement the test submission approval behavior for
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

#include "umicom/finance/regulatory/submission_approval.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSubmissionApproval record;
    CHECK(umi_reg_submission_approval_init(&record, "package_id-1", "approver_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.package_id[0] != '\0');
    CHECK(record.approver_id[0] != '\0');
    return 0;
}
