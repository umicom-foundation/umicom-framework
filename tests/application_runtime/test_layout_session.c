/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_layout_session.c
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
#include "umicom/application/runtime/layout_session.h"
int test_layout_session(void){ UmiApplicationLayoutSession s; assert(umi_application_layout_session_init(test_trader_experience(),&s)==UMI_STATUS_OK); assert(umi_application_layout_session_current(&s)!=NULL); assert(umi_application_layout_session_set_locked(&s,true)==UMI_STATUS_OK); return 0; }
