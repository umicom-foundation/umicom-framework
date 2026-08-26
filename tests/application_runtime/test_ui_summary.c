/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_summary.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_ui_summary(void){ UmiApplicationUiSummaryRow r; assert(umi_application_ui_summary_row(test_trader_experience(),&r)==UMI_STATUS_OK); assert(r.application_id!=NULL); return 0; }
