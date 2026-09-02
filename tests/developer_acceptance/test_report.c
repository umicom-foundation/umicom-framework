/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_report.c
 *
 * PURPOSE:
 *   Verify Framework developer acceptance report behaviour.
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
 * Exercise test developer acceptance report and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_developer_acceptance_report(void){UmiDeveloperAcceptanceGateResult r[2]={0};UmiDeveloperAcceptanceReport out;r[0].passed=1;r[1].missing_count=2;assert(umi_developer_acceptance_report_build(r,2,&out)==UMI_STATUS_OK);assert(out.completion_percent==50U&&out.missing_requirement_count==2U);return 0;}
