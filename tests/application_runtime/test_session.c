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
int test_session(void){ UmiApplicationSession s; const UmiApplicationExperienceDefinition *e=test_trader_experience(); assert(umi_application_session_init(e,&s)==UMI_STATUS_OK); assert(s.layout!=NULL); assert(s.active_panel_count>0U); assert(umi_application_session_validate(&s)==UMI_STATUS_OK); return 0; }
