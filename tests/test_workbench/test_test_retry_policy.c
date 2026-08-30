/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_test_retry_policy.c
 *
 * PURPOSE:
 *   Implement the test test retry policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_retry_policy.h"
#include <assert.h>
int main(void){UmiTestRetryPolicy m;assert(umi_test_retry_policy_init(&m,"test_retry_policy","Test Retry Policy")==UMI_STATUS_OK);assert(umi_test_retry_policy_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_retry_policy_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_retry_policy_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_retry_policy_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
