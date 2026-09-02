/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_statement_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the statement plan enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/statement_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataStatementPlan item;
    CHECK(umi_data_statement_plan_init(&item,"stmt1",11U,22U,2U,true) == UMI_STATUS_OK);
    CHECK(item.read_only);
    return 0;
}
