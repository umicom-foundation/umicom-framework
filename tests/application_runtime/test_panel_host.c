/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_panel_host.c
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
 * Exercise test panel host and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_panel_host(void){ UmiApplicationPanelHost h; const UmiApplicationExperienceDefinition *e=test_trader_experience(); umi_application_panel_host_init(&h); assert(umi_application_panel_host_register(&h,e,e->panels[0].panel_id,NULL)==UMI_STATUS_OK); assert(umi_application_panel_host_find(&h,e->panels[0].panel_id)!=NULL); return 0; }
