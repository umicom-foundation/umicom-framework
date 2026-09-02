/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_control_exception.c
 *
 * PURPOSE:
 *   Implement the test control exception behavior for
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

#include "umicom/finance/regulatory/control_exception.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiControlException record;
    CHECK(umi_reg_control_exception_init(&record, "exception_id-1", "control_id-1", "reason value", 1) == UMI_STATUS_OK);
    CHECK(record.exception_id[0] != '\0');
    CHECK(record.control_id[0] != '\0');
    CHECK(record.reason[0] != '\0');
    return 0;
}
