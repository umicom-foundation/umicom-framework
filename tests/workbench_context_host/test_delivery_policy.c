/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_delivery_policy.c
 *
 * PURPOSE:
 *   Verify default delivery pressure policy and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/delivery_policy.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostDeliveryPolicy policy =
        umi_workbench_context_host_delivery_policy_default();
    assert(policy.max_pending_per_endpoint == 32U);
    assert(policy.coalesce_same_context);
    assert(umi_workbench_context_host_delivery_policy_validate(
        &policy) == UMI_STATUS_OK);
    policy.max_pending_per_endpoint = 0U;
    assert(umi_workbench_context_host_delivery_policy_validate(
        &policy) == UMI_STATUS_INVALID_ARGUMENT);

    return 0;
}
