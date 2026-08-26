/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_health_ownership.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_ui_health_ownership(void){ UmiApplicationRuntimeHealth h; UmiApplicationUiHealthRow hr; UmiApplicationUiOwnershipRow o[UMI_APPLICATION_UI_OWNERSHIP_ROW_COUNT]; assert(umi_application_runtime_health_evaluate(test_trader_experience(),test_capability_probe,NULL,&h)==UMI_STATUS_OK); assert(umi_application_ui_health_row(&h,&hr)==UMI_STATUS_OK); assert(umi_application_ui_ownership_rows(test_trader_experience(),o)==UMI_STATUS_OK); return 0; }
