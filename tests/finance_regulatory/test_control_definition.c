/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_control_definition.c
 *
 * PURPOSE:
 *   Implement the test control definition behavior for
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

#include "umicom/finance/regulatory/control_definition.h"

int main(void)
{
    UmiControlDefinition record;
    CHECK(umi_reg_control_definition_init(&record, "control_id-1", "Name", "owner_id-1", 0) == UMI_STATUS_OK);
    CHECK(record.control_id[0] != '\0');
    CHECK(record.name[0] != '\0');
    CHECK(record.owner_id[0] != '\0');
    return 0;
}
