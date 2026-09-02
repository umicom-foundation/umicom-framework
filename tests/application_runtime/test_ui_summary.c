/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_summary.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
/*
 * Exercise test ui summary and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_ui_summary(void){ UmiApplicationUiSummaryRow r; assert(umi_application_ui_summary_row(test_trader_experience(),&r)==UMI_STATUS_OK); assert(r.application_id!=NULL); return 0; }
