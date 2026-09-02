/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_command_surface.c
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
 * Exercise test command surface and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_command_surface(void){ UmiApplicationCommandSurface s; assert(umi_application_command_surface_build(test_trader_experience(),&s)==UMI_STATUS_OK); assert(s.command_count>0U); return 0; }
