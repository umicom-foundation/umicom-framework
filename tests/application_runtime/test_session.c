/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_session.c
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
 * Exercise test session and return a clear result when the behaviour no longer matches its
 * contract.
 */
int test_session(void){ UmiApplicationSession s; const UmiApplicationExperienceDefinition *e=test_trader_experience(); assert(umi_application_session_init(e,&s)==UMI_STATUS_OK); assert(s.layout!=NULL); assert(s.active_panel_count>0U); assert(umi_application_session_validate(&s)==UMI_STATUS_OK); /* A damaged active-panel count must not make a query walk beyond the fixed session array. */ s.active_panel_count=UMI_APPLICATION_RUNTIME_MAX_PANELS+1U; assert(umi_application_session_validate(&s)==UMI_STATUS_INVALID_ARGUMENT); assert(!umi_application_session_panel_active(&s,"trader.chart")); return 0; }
