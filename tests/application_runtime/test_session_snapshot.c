/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_session_snapshot.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_session_snapshot(void){ UmiApplicationSession s,r; UmiApplicationSessionSnapshot snap; assert(umi_application_session_init(test_trader_experience(),&s)==UMI_STATUS_OK); assert(umi_application_session_snapshot_capture(&s,&snap)==UMI_STATUS_OK); assert(umi_application_session_snapshot_restore(test_trader_experience(),&snap,&r)==UMI_STATUS_OK); assert(r.active_panel_count==s.active_panel_count); return 0; }
