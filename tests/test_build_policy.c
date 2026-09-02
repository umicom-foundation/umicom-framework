/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_policy.c
 * PURPOSE: Verify execution policy validation and safe Ninja parallelism.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/build/policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBuildExecutionPolicy policy;
    umi_build_execution_policy_init(&policy);
    assert(umi_build_execution_policy_validate(&policy, NULL, 0U) ==
           UMI_STATUS_OK);
    assert(umi_build_policy_safe_parallel_jobs(0U, 8U, 6U) == 6U);
    assert(umi_build_policy_safe_parallel_jobs(64U, 16U, 32U) == 16U);
    assert(umi_build_policy_safe_parallel_jobs(64U, 0U, 0U) == 32U);
    policy.maximum_attempts = 0U;
    assert(umi_build_execution_policy_validate(&policy, NULL, 0U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
