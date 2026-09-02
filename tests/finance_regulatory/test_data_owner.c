/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_data_owner.c
 *
 * PURPOSE:
 *   Implement the test data owner behavior for
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

#include "umicom/finance/regulatory/data_owner.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataOwner record;
    CHECK(umi_reg_data_owner_init(&record, "owner_id-1", "domain_id-1", "Display Name", 1) == UMI_STATUS_OK);
    CHECK(record.owner_id[0] != '\0');
    CHECK(record.domain_id[0] != '\0');
    CHECK(record.display_name[0] != '\0');
    return 0;
}
