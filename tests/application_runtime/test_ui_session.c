/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_session.c
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
 * Exercise test ui session and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_ui_session(void){ UmiApplicationSession s; UmiApplicationUiSessionRow r; assert(umi_application_session_init(test_trader_experience(),&s)==UMI_STATUS_OK); assert(umi_application_ui_session_row(&s,&r)==UMI_STATUS_OK); assert(r.active_panel_count==s.active_panel_count); return 0; }
