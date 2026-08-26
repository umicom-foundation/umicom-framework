/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_readiness.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_ui_readiness(void){ UmiApplicationUiReadinessRow r[UMI_APPLICATION_UI_READINESS_ROW_COUNT]; assert(umi_application_ui_readiness_rows(test_trader_experience(),r)==UMI_STATUS_OK); return 0; }
