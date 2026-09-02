/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_health_ownership.c
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
 * Exercise test ui health ownership and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ui_health_ownership(void){ UmiApplicationRuntimeHealth h; UmiApplicationUiHealthRow hr; UmiApplicationUiOwnershipRow o[UMI_APPLICATION_UI_OWNERSHIP_ROW_COUNT]; assert(umi_application_runtime_health_evaluate(test_trader_experience(),test_capability_probe,NULL,&h)==UMI_STATUS_OK); assert(umi_application_ui_health_row(&h,&hr)==UMI_STATUS_OK); assert(umi_application_ui_ownership_rows(test_trader_experience(),o)==UMI_STATUS_OK); return 0; }
