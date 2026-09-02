/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_report_instance_store.c
 *
 * PURPOSE:
 *   Implement the test report instance store behavior for
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

#include "umicom/finance/regulatory/report_instance_store.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiReportInstanceStore s; UmiReportInstance r;
    umi_reg_report_instance_store_init(&s);
    CHECK(umi_reg_report_instance_init(&r,"r1","t1")==UMI_STATUS_OK);
    CHECK(umi_reg_report_instance_store_add(&s,&r)==UMI_STATUS_OK);
    CHECK(umi_reg_report_instance_store_find(&s,"r1") != NULL);
    return 0;
}
