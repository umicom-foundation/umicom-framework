/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_report_empty.c
 *
 * PURPOSE:
 *   Verify developer acceptance report empty behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/developer/acceptance/report.h"
/*
 * Exercise test developer acceptance report empty and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_report_empty(void)
{
    UmiDeveloperAcceptanceReport report;
    assert(umi_developer_acceptance_report_build(NULL,0U,&report)==UMI_STATUS_OK);
    assert(report.gate_count==0U);
    assert(report.completion_percent==0U);
    return 0;
}
