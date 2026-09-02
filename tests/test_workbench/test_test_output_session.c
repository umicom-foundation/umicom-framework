/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_output_session.c
 *
 * PURPOSE:
 *   Implement the test test output session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_output_session.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestOutputSession m;assert(umi_test_output_session_init(&m,"test_output_session","Test Output Session")==UMI_STATUS_OK);assert(umi_test_output_session_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_output_session_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_output_session_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_output_session_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
