/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_command_surface.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_command_surface(void){ UmiApplicationCommandSurface s; assert(umi_application_command_surface_build(test_trader_experience(),&s)==UMI_STATUS_OK); assert(s.command_count>0U); return 0; }
