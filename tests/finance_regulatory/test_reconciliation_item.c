/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reconciliation_item.c
 *
 * PURPOSE:
 *   Implement the test reconciliation item behavior for
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

#include "umicom/finance/regulatory/reconciliation_item.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReconciliationItem record;
    CHECK(umi_reg_reconciliation_item_init(&record, "item_id-1", 1.25, 1.25, 1.25) == UMI_STATUS_OK);
    CHECK(record.item_id[0] != '\0');
    return 0;
}
