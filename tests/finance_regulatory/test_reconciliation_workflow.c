/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reconciliation_workflow.c
 *
 * PURPOSE:
 *   Implement the test reconciliation workflow behavior for
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

#include "umicom/finance/regulatory/reconciliation_workflow.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReconciliationWorkflow record;
    CHECK(umi_reg_reconciliation_workflow_init(&record, "workflow_id-1", "break_id-1", "owner_id-1", 1) == UMI_STATUS_OK);
    CHECK(record.workflow_id[0] != '\0');
    CHECK(record.break_id[0] != '\0');
    CHECK(record.owner_id[0] != '\0');
    return 0;
}
