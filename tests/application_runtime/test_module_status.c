/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_module_status.c
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
 * Exercise test module status and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_module_status(void){ UmiApplicationModuleStatus s; assert(umi_application_module_status_init(test_trader_experience(),1,1,1,&s)==UMI_STATUS_OK); assert(umi_application_module_status_runnable(&s)); return 0; }
