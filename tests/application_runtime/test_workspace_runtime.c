/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_workspace_runtime.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_workspace_runtime(void){ UmiApplicationWorkspaceRuntime r; assert(umi_application_workspace_runtime_init(test_trader_experience(),&r)==UMI_STATUS_OK); assert(r.session.active_panel_count>0U); assert(r.commands.command_count>0U); return 0; }
