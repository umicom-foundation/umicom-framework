/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_regulatory_authority.c
 *
 * PURPOSE:
 *   Implement the test regulatory authority behavior for
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

#include "umicom/finance/regulatory/regulatory_authority.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRegulatoryAuthority record;
    CHECK(umi_reg_regulatory_authority_init(&record, "authority_id-1", "Name", "Jurisdiction", 1) == UMI_STATUS_OK);
    CHECK(record.authority_id[0] != '\0');
    CHECK(record.name[0] != '\0');
    CHECK(record.jurisdiction[0] != '\0');
    return 0;
}
