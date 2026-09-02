/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_control_execution.c
 *
 * PURPOSE:
 *   Implement the test control execution behavior for
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

#include "umicom/finance/regulatory/control_execution.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiControlExecution record;
    CHECK(umi_reg_control_execution_init(&record, "execution_id-1", "control_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.execution_id[0] != '\0');
    CHECK(record.control_id[0] != '\0');
    return 0;
}
