/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_panel_state.c
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
#include "umicom/application/runtime/panel_state.h"
/*
 * Exercise test panel state and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_panel_state(void){ UmiApplicationPanelState s; const UmiExperiencePanelDefinition *p=&test_trader_experience()->panels[0]; assert(umi_application_panel_state_init(p,&s)==UMI_STATUS_OK); assert(s.definition==p); return 0; }
