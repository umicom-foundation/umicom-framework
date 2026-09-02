/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_readiness.c
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
 * Exercise test ui readiness and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ui_readiness(void){ UmiApplicationUiReadinessRow r[UMI_APPLICATION_UI_READINESS_ROW_COUNT]; assert(umi_application_ui_readiness_rows(test_trader_experience(),r)==UMI_STATUS_OK); return 0; }
