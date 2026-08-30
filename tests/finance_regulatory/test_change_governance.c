/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_change_governance.c
 *
 * PURPOSE:
 *   Implement the test change governance behavior for
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

#include "umicom/finance/regulatory/change_governance.h"

int main(void)
{
    UmiChangeGovernance record;
    CHECK(umi_reg_change_governance_init(&record, "change_id-1", "approver_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.change_id[0] != '\0');
    CHECK(record.approver_id[0] != '\0');
    return 0;
}
