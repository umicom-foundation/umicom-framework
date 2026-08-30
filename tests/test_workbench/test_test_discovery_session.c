/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_discovery_session.c
 *
 * PURPOSE:
 *   Implement the test test discovery session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_discovery_session.h"
#include <assert.h>
int main(void){UmiTestDiscoverySession m;assert(umi_test_discovery_session_init(&m,"test_discovery_session","Test Discovery Session")==UMI_STATUS_OK);assert(umi_test_discovery_session_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_discovery_session_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_discovery_session_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_discovery_session_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
